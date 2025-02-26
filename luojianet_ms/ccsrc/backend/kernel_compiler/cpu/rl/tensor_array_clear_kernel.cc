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

#include "backend/kernel_compiler/cpu/rl/tensor_array_clear_kernel.h"
#include "backend/kernel_compiler/common_utils.h"
#include "runtime/device/cpu/cpu_tensor_array.h"
#include "runtime/device/tensor_array_manager.h"

namespace luojianet_ms {
namespace kernel {
using luojianet_ms::device::TensorArrayMgr;
using luojianet_ms::device::TensorArrayPtr;
TensorArrayCPUClearKernel::TensorArrayCPUClearKernel() {}

const std::vector<size_t> &TensorArrayCPUClearKernel::GetInputSizeList() const { return input_size_list_; }

const std::vector<size_t> &TensorArrayCPUClearKernel::GetOutputSizeList() const { return output_size_list_; }

const std::vector<size_t> &TensorArrayCPUClearKernel::GetWorkspaceSizeList() const { return workspace_size_list_; }

void TensorArrayCPUClearKernel::InitKernel(const CNodePtr &kernel_node) {
  MS_EXCEPTION_IF_NULL(kernel_node);
  input_size_list_.push_back(sizeof(int64_t));
  output_size_list_.push_back(sizeof(int64_t));
}

bool TensorArrayCPUClearKernel::Launch(const std::vector<AddressPtr> &inputs, const std::vector<AddressPtr> &,
                                       const std::vector<AddressPtr> &) {
  auto handle_addr = GetDeviceAddress<int64_t>(inputs, 0);
  MS_EXCEPTION_IF_NULL(handle_addr);
  TensorArrayPtr tensors_ = TensorArrayMgr::GetInstance().GetTensorArray(handle_addr[0]);
  MS_ERROR_IF_NULL(tensors_);
  // Clear TensorArray valid size, but keep the memory.
  tensors_->Clear();
  return true;
}
}  // namespace kernel
}  // namespace luojianet_ms
