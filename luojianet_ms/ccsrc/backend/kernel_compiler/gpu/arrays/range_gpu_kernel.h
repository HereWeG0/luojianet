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

#ifndef LUOJIANET_MS_CCSRC_BACKEND_KERNEL_COMPILER_GPU_RANGE_GPU_KERNEL_H_
#define LUOJIANET_MS_CCSRC_BACKEND_KERNEL_COMPILER_GPU_RANGE_GPU_KERNEL_H_
#include <vector>
#include "backend/kernel_compiler/gpu/gpu_kernel.h"
#include "backend/kernel_compiler/gpu/gpu_kernel_factory.h"
#include "backend/kernel_compiler/gpu/cuda_impl/range_impl.cuh"
namespace luojianet_ms {
namespace kernel {
template <typename T>
class RangeGPUKernel : public GpuKernel {
 public:
  RangeGPUKernel() : input_size_(0), output_size_(0), start_(0.), limit_(1.), delta_(1.), is_null_input_(false) {}
  ~RangeGPUKernel() = default;

  const std::vector<size_t> &GetInputSizeList() const override { return input_size_list_; }
  const std::vector<size_t> &GetOutputSizeList() const override { return output_size_list_; }
  const std::vector<size_t> &GetWorkspaceSizeList() const override { return workspace_size_list_; }

  bool Launch(const std::vector<AddressPtr> &inputs, const std::vector<AddressPtr> &workspace,
              const std::vector<AddressPtr> &outputs, void *stream_ptr) override {
    if (is_null_input_) {
      return true;
    }
    T *input = GetDeviceAddress<T>(inputs, 0);
    T *output = GetDeviceAddress<T>(outputs, 0);
    int size = SizeToInt(input_size_ / sizeof(T));
    CalRange(size, start_, limit_, delta_, input, output, reinterpret_cast<cudaStream_t>(stream_ptr));
    return true;
  }

  bool Init(const CNodePtr &kernel_node) override {
    auto kernel_name = AnfAlgo::GetCNodeName(kernel_node);
    size_t input_num = AnfAlgo::GetInputTensorNum(kernel_node);
    if (input_num != 1) {
      MS_LOG(EXCEPTION) << "For '" << kernel_name << "', the number of inputs should be 1, but got " << input_num;
    }
    size_t output_num = AnfAlgo::GetOutputTensorNum(kernel_node);
    if (output_num != 1) {
      MS_LOG(EXCEPTION) << "For '" << kernel_name << "', the number of outputs should be 1, but got " << output_num;
    }
    auto input_shape = AnfAlgo::GetInputDeviceShape(kernel_node, 0);
    is_null_input_ = CHECK_SHAPE_NULL(input_shape, kernel_name, "input");
    if (is_null_input_) {
      InitSizeLists();
      return true;
    }
    auto shape_size = input_shape.size();
    input_size_ = 1;
    for (size_t i = 0; i < shape_size; i++) {
      input_size_ *= input_shape[i];
    }
    input_size_ *= sizeof(T);
    output_size_ = input_size_;
    start_ = GetAttr<float>(kernel_node, "start");
    limit_ = GetAttr<float>(kernel_node, "limit");
    delta_ = GetAttr<float>(kernel_node, "delta");
    InitSizeLists();
    return true;
  }

 protected:
  void InitSizeLists() override {
    input_size_list_.push_back(input_size_);
    output_size_list_.push_back(output_size_);
    return;
  }

 private:
  std::vector<size_t> input_size_list_;
  std::vector<size_t> output_size_list_;
  std::vector<size_t> workspace_size_list_;
  size_t input_size_;
  size_t output_size_;
  float start_;
  float limit_;
  float delta_;
  bool is_null_input_;
};
}  // namespace kernel
}  // namespace luojianet_ms
#endif  // LUOJIANET_MS_CCSRC_BACKEND_KERNEL_COMPILER_GPU_RANGE_GPU_KERNEL_H_
