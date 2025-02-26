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

#ifndef LUOJIANET_MS_CCSRC_BACKEND_KERNEL_COMPILER_GPU_NN_BINARY_CROSS_ENTROPY_GRAD_KERNEL_H
#define LUOJIANET_MS_CCSRC_BACKEND_KERNEL_COMPILER_GPU_NN_BINARY_CROSS_ENTROPY_GRAD_KERNEL_H

#include <string>
#include <vector>
#include "backend/kernel_compiler/gpu/gpu_kernel.h"
#include "backend/kernel_compiler/gpu/gpu_kernel_factory.h"
#include "backend/kernel_compiler/gpu/cuda_impl/loss_with_reduction_impl.cuh"
#include "backend/kernel_compiler/common_utils.h"

namespace luojianet_ms {
namespace kernel {
template <typename T>
class BinaryCrossEntropyGradGpuKernel : public GpuKernel {
 public:
  BinaryCrossEntropyGradGpuKernel()
      : input_size_(1),
        reduction_(ReductionMode::kMean),
        weight_defined_(false),
        is_null_input_(false),
        kernel_name_("BinaryCrossEntropyGrad") {}
  ~BinaryCrossEntropyGradGpuKernel() override = default;

  const std::vector<size_t> &GetInputSizeList() const override { return input_size_list_; }
  const std::vector<size_t> &GetOutputSizeList() const override { return output_size_list_; }
  const std::vector<size_t> &GetWorkspaceSizeList() const override { return workspace_size_list_; }

  bool Launch(const std::vector<AddressPtr> &inputs, const std::vector<AddressPtr> &,
              const std::vector<AddressPtr> &outputs, void *stream_ptr) override {
    if (is_null_input_) {
      return true;
    }
    T *input_x = GetDeviceAddress<T>(inputs, 0);
    T *input_y = GetDeviceAddress<T>(inputs, 1);
    T *dloss = GetDeviceAddress<T>(inputs, 2);
    T *weight = nullptr;
    if (weight_defined_) {
      weight = GetDeviceAddress<T>(inputs, 3);
    }
    T *dx = GetDeviceAddress<T>(outputs, 0);
    if (input_size_ > 0) {
      BinaryCrossEntropyLossGrad(input_size_, reduction_, input_x, input_y, weight, dloss, dx,
                                 reinterpret_cast<cudaStream_t>(stream_ptr));
    }
    return true;
  }

  bool Init(const CNodePtr &kernel_node) override {
    kernel_name_ = AnfAlgo::GetCNodeName(kernel_node);
    auto input_shape = AnfAlgo::GetPrevNodeOutputInferShape(kernel_node, 0);
    is_null_input_ = CHECK_SHAPE_NULL(input_shape, kernel_name_, "input");
    if (is_null_input_) {
      InitSizeLists();
      return true;
    }
    size_t input_num = AnfAlgo::GetInputTensorNum(kernel_node);
    weight_defined_ = (input_num == 4);
    for (size_t i = 0; i < input_shape.size(); i++) {
      input_size_ *= input_shape[i];
    }
    string reduction = GetAttr<string>(kernel_node, "reduction");
    reduction_ = kReductionModeMap[reduction];
    InitSizeLists();
    return true;
  }

 protected:
  void InitSizeLists() override {
    input_size_list_.push_back(input_size_ * sizeof(T));
    input_size_list_.push_back(input_size_ * sizeof(T));
    if (reduction_ == ReductionMode::kNone) {
      input_size_list_.push_back(input_size_ * sizeof(T));
    } else {
      input_size_list_.push_back(sizeof(T));
    }
    if (weight_defined_) {
      input_size_list_.push_back(input_size_ * sizeof(T));
    }
    output_size_list_.push_back(input_size_ * sizeof(T));
  }

 private:
  size_t input_size_;
  ReductionMode reduction_;
  bool weight_defined_;  // true: there are 4 inputs, false: there are 3 inputs(no [weight])
  bool is_null_input_;
  std::string kernel_name_;
  std::vector<size_t> input_size_list_;
  std::vector<size_t> output_size_list_;
  std::vector<size_t> workspace_size_list_;
};
}  // namespace kernel
}  // namespace luojianet_ms
#endif  // LUOJIANET_MS_BINARY_CROSS_ENTROPY_GRAD_KERNEL_H
