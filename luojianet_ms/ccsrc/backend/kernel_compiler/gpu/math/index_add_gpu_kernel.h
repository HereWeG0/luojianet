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

#ifndef LUOJIANET_MS_CCSRC_BACKEND_KERNEL_COMPILER_GPU_MATH_INDEX_ADD_GPU_KERNEL_H_
#define LUOJIANET_MS_CCSRC_BACKEND_KERNEL_COMPILER_GPU_MATH_INDEX_ADD_GPU_KERNEL_H_

#include <vector>
#include "backend/kernel_compiler/gpu/gpu_kernel.h"
#include "backend/kernel_compiler/gpu/gpu_kernel_factory.h"
#include "backend/kernel_compiler/gpu/cuda_impl/index_add_impl.cuh"
namespace luojianet_ms {
namespace kernel {
template <typename T>
class IndexAddGpuKernel : public GpuKernel {
 public:
  IndexAddGpuKernel()
      : dst_size_(0),
        index_size_(0),
        src_size_(0),
        output_size_(0),
        outer_size_(0),
        src_axis_size_(0),
        dst_axis_size_(0),
        inner_size_(0),
        use_lock_(true),
        is_null_input_(false) {}
  ~IndexAddGpuKernel() override = default;

  const std::vector<size_t> &GetInputSizeList() const override { return input_size_list_; }
  const std::vector<size_t> &GetOutputSizeList() const override { return output_size_list_; }
  const std::vector<size_t> &GetWorkspaceSizeList() const override { return workspace_size_list_; }

  bool Launch(const std::vector<AddressPtr> &inputs, const std::vector<AddressPtr> &workspace,
              const std::vector<AddressPtr> &outputs, void *stream_ptr) override {
    if (is_null_input_) {
      return true;
    }
    T *dst = GetDeviceAddress<T>(inputs, 0);
    int *index = GetDeviceAddress<int>(inputs, 1);
    T *src = GetDeviceAddress<T>(inputs, 2);
    T *dst_out = GetDeviceAddress<T>(outputs, 0);
    CalIndexAdd(dst, index, src, outer_size_, src_axis_size_, dst_axis_size_, inner_size_, use_lock_,
                reinterpret_cast<cudaStream_t>(stream_ptr));
    CHECK_CUDA_RET_WITH_EXCEPT(kernel_node_,
                               cudaMemcpyAsync(&dst_out[0], &dst[0], dst_size_, cudaMemcpyDeviceToDevice,
                                               reinterpret_cast<cudaStream_t>(stream_ptr)),
                               "cudaMemcpyAsync output failed");
    return true;
  }

  bool Init(const CNodePtr &kernel_node) override {
    kernel_node_ = kernel_node;
    size_t input_num = AnfAlgo::GetInputTensorNum(kernel_node);
    if (input_num != 3) {
      MS_LOG(ERROR) << "Input number is " << input_num << ", but index add needs 3 inputs.";
      return false;
    }
    std::vector<size_t> dst_shape = AnfAlgo::GetPrevNodeOutputInferShape(kernel_node, 0);
    std::vector<size_t> index_shape = AnfAlgo::GetPrevNodeOutputInferShape(kernel_node, 1);
    std::vector<size_t> src_shape = AnfAlgo::GetPrevNodeOutputInferShape(kernel_node, 2);
    is_null_input_ = CHECK_NULL_INPUT(dst_shape) || CHECK_NULL_INPUT(index_shape) || CHECK_NULL_INPUT(src_shape);
    if (is_null_input_) {
      MS_LOG(WARNING) << "For 'IndexAddGpuKernel', input is null";
      InitSizeLists();
      return true;
    }
    int64_t src_rank = src_shape.size();
    int64_t axis = GetAttr<int64_t>(kernel_node, "axis");
    if (axis < 0) {
      axis += src_rank;
    }
    outer_size_ = 1;
    for (int64_t i = axis - 1; i >= 0; i--) {
      outer_size_ *= src_shape[i];
    }
    inner_size_ = 1;
    for (int64_t i = axis + 1; i >= 0 && i < src_rank; i++) {
      inner_size_ *= src_shape[i];
    }
    if (axis < 0 || axis >= SizeToInt(src_shape.size()) || axis >= SizeToInt(dst_shape.size())) {
      MS_LOG(EXCEPTION) << "Init axis size failed, actual src axis size is " << src_axis_size_
                        << ", actual dst axis size is " << dst_axis_size_;
    }
    src_axis_size_ = src_shape[axis];
    dst_axis_size_ = dst_shape[axis];

    dst_size_ = sizeof(T);
    for (auto x : dst_shape) {
      dst_size_ *= x;
    }
    index_size_ = sizeof(int);
    for (auto x : index_shape) {
      index_size_ *= x;
    }
    src_size_ = sizeof(T);
    for (auto x : src_shape) {
      src_size_ *= x;
    }
    output_size_ = dst_size_;
    InitSizeLists();
    return true;
  }

 protected:
  void InitSizeLists() override {
    input_size_list_.push_back(dst_size_);
    input_size_list_.push_back(index_size_);
    input_size_list_.push_back(src_size_);
    output_size_list_.push_back(output_size_);
  }

 private:
  size_t dst_size_;
  size_t index_size_;
  size_t src_size_;
  size_t output_size_;
  size_t outer_size_;
  size_t src_axis_size_;
  size_t dst_axis_size_;
  size_t inner_size_;
  bool use_lock_;
  bool is_null_input_;
  std::vector<size_t> input_size_list_;
  std::vector<size_t> output_size_list_;
  std::vector<size_t> workspace_size_list_;
};
}  // namespace kernel
}  // namespace luojianet_ms

#endif  // LUOJIANET_MS_CCSRC_BACKEND_KERNEL_COMPILER_GPU_MATH_INDEX_ADD_GPU_KERNEL_H_
