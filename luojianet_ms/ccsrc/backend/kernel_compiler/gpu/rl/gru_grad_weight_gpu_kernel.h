/**
 * Copyright 2021, 2022 LuoJiaNET Research and Development Group, Wuhan University
 * Copyright 2021, 2022 Huawei Technologies Co., Ltd
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef LUOJIANET_MS_CCSRC_BACKEND_KERNEL_COMPILER_GPU_NN_GRU_GRAD_WEIGHT_GPU_KERNEL_H_
#define LUOJIANET_MS_CCSRC_BACKEND_KERNEL_COMPILER_GPU_NN_GRU_GRAD_WEIGHT_GPU_KERNEL_H_

#include <cuda_runtime_api.h>
#include <vector>
#include <memory>
#include "backend/kernel_compiler/gpu/gpu_kernel.h"
#include "backend/kernel_compiler/gpu/gpu_kernel_factory.h"
#include "backend/kernel_compiler/gpu/kernel_constants.h"
namespace luojianet_ms {
namespace kernel {
constexpr size_t kIndexFour = 4;
constexpr size_t DimOfTensor = 3;
constexpr size_t LeastWeightShape = 3;
constexpr size_t LeastInputShapeSize = 2;
template <typename T>
class GruGradWeightGpuKernel : public GpuKernel {
 public:
  GruGradWeightGpuKernel()
      : batch_size_(0),
        seq_len_(0),
        input_size_(0),
        hidden_size_(0),
        num_layers_(0),
        has_bias_(false),
        bidirectional_(false),
        states_init_(false),
        is_null_input_(false),
        dropout_(0),
        weight_size_(0),
        reserved_size_(0),
        rnn_desc_(nullptr),
        dropout_desc_(nullptr),
        x_desc_(nullptr),
        hx_desc_(nullptr),
        y_desc_(nullptr),
        dw_desc_(nullptr),
        handle_(nullptr),
        cudnn_data_type_(CUDNN_DATA_FLOAT) {}
  ~GruGradWeightGpuKernel() override { DestroyResource(); }

  const std::vector<size_t> &GetInputSizeList() const override { return input_size_list_; }
  const std::vector<size_t> &GetOutputSizeList() const override { return output_size_list_; }
  const std::vector<size_t> &GetWorkspaceSizeList() const override { return workspace_size_list_; }
  bool Launch(const std::vector<AddressPtr> &inputs, const std::vector<AddressPtr> &workspace,
              const std::vector<AddressPtr> &outputs, void *stream_ptr) override {
    if (is_null_input_) {
      return true;
    }
    auto x_addr = GetDeviceAddress<T>(inputs, 0);
    auto hx_addr = GetDeviceAddress<T>(inputs, 1);
    auto y_addr = GetDeviceAddress<T>(inputs, 2);
    auto reserved_addr = GetDeviceAddress<T>(inputs, 3);
    auto states_addr = GetDeviceAddress<T>(inputs, 4);
    auto dw_addr = GetDeviceAddress<T>(outputs, 0);
    void *workspace_addr = GetDeviceAddress<T>(workspace, 0);

    if (!states_init_) {
      CHECK_CUDNN_RET_WITH_EXCEPT(
        kernel_node_,
        cudnnRestoreDropoutDescriptor(dropout_desc_, handle_, dropout_, states_addr, input_size_list_[kIndexFour], 0),
        "restore dropout state failed");
      states_init_ = true;
    }

    CHECK_CUDA_RET_WITH_EXCEPT(
      kernel_node_, cudaMemsetAsync(dw_addr, 0, outputs[0]->size, reinterpret_cast<cudaStream_t>(stream_ptr)),
      "cudaMemSet Failed");

    CHECK_CUDNN_RET_WITH_EXCEPT(
      kernel_node_,
      cudnnRNNBackwardWeights(handle_, rnn_desc_, seq_len_, x_desc_.get(), x_addr, hx_desc_, hx_addr, y_desc_.get(),
                              y_addr, workspace_addr, workspace_size_list_[0], dw_desc_, dw_addr, reserved_addr,
                              reserved_size_),
      "launch gru back weight kernel failed");

    return true;
  }
  bool Init(const CNodePtr &kernel_node) override {
    auto kernel_name = AnfAlgo::GetCNodeName(kernel_node);
    kernel_node_ = kernel_node;
    InitResource();
    cudnn_data_type_ = GetCudnnDataType(TypeIdLabel(AnfAlgo::GetInputDeviceDataType(kernel_node, 0)));
    auto input_shape = AnfAlgo::GetPrevNodeOutputInferShape(kernel_node, 0);
    is_null_input_ = CHECK_SHAPE_NULL(input_shape, kernel_name, "input");
    if (is_null_input_) {
      InitSizeLists();
      return true;
    }
    if (input_shape.size() < LeastInputShapeSize) {
      MS_LOG(EXCEPTION) << "For '" << kernel_name << "', the dimension of input cannot be less than 2, but got "
                        << input_shape.size();
    }
    seq_len_ = SizeToInt(input_shape[0]);
    batch_size_ = SizeToInt(input_shape[1]);

    input_size_ = static_cast<int>(GetAttr<int64_t>(kernel_node, "input_size"));
    hidden_size_ = static_cast<int>(GetAttr<int64_t>(kernel_node, "hidden_size"));
    num_layers_ = static_cast<int>(GetAttr<int64_t>(kernel_node, "num_layers"));
    has_bias_ = GetAttr<bool>(kernel_node, "has_bias");
    bidirectional_ = GetAttr<bool>(kernel_node, "bidirectional");
    dropout_ = GetAttr<float>(kernel_node, "dropout");

    cudnnRNNInputMode_t input_mode = CUDNN_LINEAR_INPUT;
    cudnnDirectionMode_t direction = bidirectional_ ? CUDNN_BIDIRECTIONAL : CUDNN_UNIDIRECTIONAL;
    cudnnRNNMode_t rnn_mode = CUDNN_GRU;
    cudnnRNNAlgo_t algo = CUDNN_RNN_ALGO_STANDARD;

    CreateTensorDescGrp();
    int hx_dims[3]{num_layers_ * (bidirectional_ ? 2 : 1), batch_size_, hidden_size_};
    CHECK_CUDNN_RET_WITH_EXCEPT(
      kernel_node_, cudnnSetTensorNdDescriptorEx(hx_desc_, CUDNN_TENSOR_NCHW, cudnn_data_type_, DimOfTensor, hx_dims),
      "set hx_desc_ failed");
    CHECK_CUDNN_RET_WITH_EXCEPT(kernel_node_,
                                cudnnSetDropoutDescriptor(dropout_desc_, handle_, dropout_, nullptr, 0, 0),
                                "set dropout_desc failed");
    cudnnRNNBiasMode_t bias_mode = has_bias_ ? CUDNN_RNN_DOUBLE_BIAS : CUDNN_RNN_NO_BIAS;
#if CUDNN_VERSION < 8000
    CHECK_CUDNN_RET_WITH_EXCEPT(kernel_node_,
                                cudnnSetRNNDescriptor_v6(handle_, rnn_desc_, hidden_size_, num_layers_, dropout_desc_,
                                                         input_mode, direction, rnn_mode, algo, cudnn_data_type_),
                                "set rnn_desc failed");
    CHECK_CUDNN_RET_WITH_EXCEPT(kernel_node_, cudnnSetRNNBiasMode(rnn_desc_, bias_mode), "set bias_mode failed");
#else
    cudnnMathType_t math_type = (cudnn_data_type_ == CUDNN_DATA_HALF) ? CUDNN_TENSOR_OP_MATH : CUDNN_FMA_MATH;
    CHECK_CUDNN_RET_WITH_EXCEPT(kernel_node_,
                                cudnnSetRNNDescriptor_v8(rnn_desc_, algo, rnn_mode, bias_mode, direction, input_mode,
                                                         cudnn_data_type_, cudnn_data_type_, math_type, input_size_,
                                                         hidden_size_, hidden_size_, num_layers_, dropout_desc_, 0),
                                "set rnn_desc failed");
#endif
    auto weight_shape = AnfAlgo::GetOutputInferShape(kernel_node, 0);
    is_null_input_ = CHECK_SHAPE_NULL(weight_shape, kernel_name, "weight");
    if (is_null_input_) {
      InitSizeLists();
      return true;
    }
    if (weight_shape.size() < LeastWeightShape) {
      MS_LOG(EXCEPTION) << "For '" << kernel_name << "', the dimension of weight cannot be less than 3, but got "
                        << weight_shape.size();
    }
    size_t weight_size = weight_shape[0] * weight_shape[1] * weight_shape[2] * sizeof(T);

    CHECK_CUDNN_RET_WITH_EXCEPT(kernel_node_,
                                cudnnGetRNNParamsSize(handle_, rnn_desc_, x_desc_[0], &weight_size_, cudnn_data_type_),
                                "get weight_size_ failed");
    if (weight_size != weight_size_) {
      MS_LOG(EXCEPTION) << "For '" << kernel_name << "', the size of weight should be equal to " << weight_size_
                        << " but got " << weight_size;
    }
    int w_dims[3] = {SizeToInt(weight_size_ / sizeof(T)), 1, 1};
    CHECK_CUDNN_RET_WITH_EXCEPT(
      kernel_node_, cudnnSetFilterNdDescriptor(dw_desc_, cudnn_data_type_, CUDNN_TENSOR_NCHW, DimOfTensor, w_dims),
      "set dw_desc failed");
    CHECK_CUDNN_RET_WITH_EXCEPT(
      kernel_node_, cudnnGetRNNTrainingReserveSize(handle_, rnn_desc_, seq_len_, x_desc_.get(), &reserved_size_),
      "get reserve size failed");
    InitSizeLists();
    return true;
  }

 protected:
  void InitResource() override {
    handle_ = device::gpu::GPUDeviceManager::GetInstance().GetCudnnHandle();
    CHECK_CUDNN_RET_WITH_EXCEPT(kernel_node_, cudnnCreateTensorDescriptor(&hx_desc_), "create hx_desc_ failed");
    CHECK_CUDNN_RET_WITH_EXCEPT(kernel_node_, cudnnCreateFilterDescriptor(&dw_desc_), "create dw_desc_ failed");
    CHECK_CUDNN_RET_WITH_EXCEPT(kernel_node_, cudnnCreateDropoutDescriptor(&dropout_desc_),
                                "create dropout_desc failed");
    CHECK_CUDNN_RET_WITH_EXCEPT(kernel_node_, cudnnCreateRNNDescriptor(&rnn_desc_), "create rnn_desc failed");
  }
  void InitSizeLists() override {
    size_t x_size = IntToSize(seq_len_ * batch_size_ * input_size_) * sizeof(T);

    size_t h_size = 0;
    CHECK_CUDNN_RET_WITH_EXCEPT(kernel_node_, cudnnGetTensorSizeInBytes(hx_desc_, &h_size), "get h size failed");

    size_t y_size = IntToSize(seq_len_ * batch_size_ * hidden_size_ * (bidirectional_ ? 2 : 1)) * sizeof(T);
    input_size_list_.push_back(x_size);
    input_size_list_.push_back(h_size);
    input_size_list_.push_back(y_size);
    input_size_list_.push_back(reserved_size_);
    size_t state_size = 0;
    CHECK_CUDNN_RET_WITH_EXCEPT(kernel_node_, cudnnDropoutGetStatesSize(handle_, &state_size),
                                "get dropout states size failed");
    input_size_list_.push_back(state_size);

    output_size_list_.push_back(weight_size_);

    size_t workspace_size = 0;
    CHECK_CUDNN_RET_WITH_EXCEPT(kernel_node_,
                                cudnnGetRNNWorkspaceSize(handle_, rnn_desc_, seq_len_, x_desc_.get(), &workspace_size),
                                "get workspace size failed");
    workspace_size_list_.push_back(workspace_size);
  }
  void DestroyResource() noexcept override {
    CHECK_CUDNN_RET_WITH_ERROR(kernel_node_, cudnnDestroyRNNDescriptor(rnn_desc_), "destroy rnn_desc failed");
    CHECK_CUDNN_RET_WITH_ERROR(kernel_node_, cudnnDestroyDropoutDescriptor(dropout_desc_),
                               "destroy dropout_desc failed");
    CHECK_CUDNN_RET_WITH_ERROR(kernel_node_, cudnnDestroyFilterDescriptor(dw_desc_), "destroy dw_desc_ failed");
    CHECK_CUDNN_RET_WITH_ERROR(kernel_node_, cudnnDestroyTensorDescriptor(hx_desc_), "destroy hx_desc_ failed");
    DestroyTensorDescGrp();
  }

 private:
  void CreateTensorDescGrp() {
    int x_dims[3]{batch_size_, input_size_, 1};
    int y_dims[3]{batch_size_, hidden_size_ * (bidirectional_ ? 2 : 1), 1};

    x_desc_ = std::make_unique<cudnnTensorDescriptor_t[]>(seq_len_);
    y_desc_ = std::make_unique<cudnnTensorDescriptor_t[]>(seq_len_);

    for (size_t i = 0; i < IntToSize(seq_len_); ++i) {
      CHECK_CUDNN_RET_WITH_EXCEPT(kernel_node_, cudnnCreateTensorDescriptor(&x_desc_[i]), "create x_desc failed");
      CHECK_CUDNN_RET_WITH_EXCEPT(
        kernel_node_,
        cudnnSetTensorNdDescriptorEx(x_desc_[i], CUDNN_TENSOR_NCHW, cudnn_data_type_, DimOfTensor, x_dims),
        "set x_desc failed");

      CHECK_CUDNN_RET_WITH_EXCEPT(kernel_node_, cudnnCreateTensorDescriptor(&y_desc_[i]), "create y_desc failed");
      CHECK_CUDNN_RET_WITH_EXCEPT(
        kernel_node_,
        cudnnSetTensorNdDescriptorEx(y_desc_[i], CUDNN_TENSOR_NCHW, cudnn_data_type_, DimOfTensor, y_dims),
        "set y_desc failed");
    }
  }
  void DestroyTensorDescGrp() {
    for (size_t i = 0; i < IntToSize(seq_len_); ++i) {
      CHECK_CUDNN_RET_WITH_ERROR(kernel_node_, cudnnDestroyTensorDescriptor(y_desc_[i]), "destroy y_desc failed");
      CHECK_CUDNN_RET_WITH_ERROR(kernel_node_, cudnnDestroyTensorDescriptor(x_desc_[i]), "destroy x_desc failed");
    }
  }

  int batch_size_;
  int seq_len_;
  int input_size_;
  int hidden_size_;
  int num_layers_;

  bool has_bias_;
  bool bidirectional_;
  bool states_init_;
  bool is_null_input_;
  float dropout_;

  size_t weight_size_;
  size_t reserved_size_;

  cudnnRNNDescriptor_t rnn_desc_;
  cudnnDropoutDescriptor_t dropout_desc_;

  // input desc
  std::unique_ptr<cudnnTensorDescriptor_t[]> x_desc_;
  cudnnTensorDescriptor_t hx_desc_;
  std::unique_ptr<cudnnTensorDescriptor_t[]> y_desc_;

  // output desc
  cudnnFilterDescriptor_t dw_desc_;

  cudnnHandle_t handle_;
  cudnnDataType_t cudnn_data_type_;
  std::vector<size_t> input_size_list_;
  std::vector<size_t> output_size_list_;
  std::vector<size_t> workspace_size_list_;
};
}  // namespace kernel
}  // namespace luojianet_ms
#endif  // LUOJIANET_MS_CCSRC_BACKEND_KERNEL_COMPILER_GPU_NN_GRU_GRAD_WEIGHT_GPU_KERNEL_H_
