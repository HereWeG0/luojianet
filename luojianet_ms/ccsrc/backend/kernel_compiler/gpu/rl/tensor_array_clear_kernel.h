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
#ifndef LUOJIANET_MS_CCSRC_BACKEND_KERNEL_COMPILER_GPU_RL_TENSOR_ARRAY_CLEAR_KERNEL_H_
#define LUOJIANET_MS_CCSRC_BACKEND_KERNEL_COMPILER_GPU_RL_TENSOR_ARRAY_CLEAR_KERNEL_H_

#include <vector>
#include <string>
#include "backend/kernel_compiler/gpu/gpu_kernel.h"
#include "backend/kernel_compiler/gpu/gpu_kernel_factory.h"

namespace luojianet_ms {
namespace kernel {
class TensorArrayClearKernel : public GpuKernel {
 public:
  TensorArrayClearKernel();
  ~TensorArrayClearKernel() = default;

  const std::vector<size_t> &GetInputSizeList() const override;
  const std::vector<size_t> &GetOutputSizeList() const override;
  const std::vector<size_t> &GetWorkspaceSizeList() const override;
  bool Launch(const std::vector<AddressPtr> &inputs, const std::vector<AddressPtr> &workspace,
              const std::vector<AddressPtr> &outputs, void *stream_ptr) override;
  bool Init(const CNodePtr &kernel_node) override;

 protected:
  void InitSizeLists() override;

 private:
  std::vector<size_t> input_size_list_;
  std::vector<size_t> output_size_list_;
  std::vector<size_t> workspace_size_list_;
};

MS_REG_GPU_KERNEL(TensorArrayClear, TensorArrayClearKernel)
}  // namespace kernel
}  // namespace luojianet_ms

#endif  // LUOJIANET_MS_CCSRC_BACKEND_KERNEL_COMPILER_GPU_RL_TENSOR_ARRAY_CLEAR_KERNEL_H_
