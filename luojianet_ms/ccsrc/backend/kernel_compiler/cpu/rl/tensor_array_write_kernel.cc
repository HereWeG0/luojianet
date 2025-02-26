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
#include "backend/kernel_compiler/cpu/rl/tensor_array_write_kernel.h"
#include <memory>
#include "backend/kernel_compiler/common_utils.h"
#include "runtime/device/cpu/cpu_tensor_array.h"
#include "runtime/device/tensor_array_manager.h"
#include "runtime/hardware/cpu/cpu_memory_pool.h"
namespace luojianet_ms {
namespace kernel {
constexpr size_t kSecondInputIndex = 2;
using luojianet_ms::device::TensorArrayMgr;
using luojianet_ms::device::cpu::CPUTensorArray;
using luojianet_ms::device::cpu::CPUTensorArrayPtr;
TensorArrayCPUWriteKernel::TensorArrayCPUWriteKernel() : value_size_(0), type_(kTypeUnknown) {}

const std::vector<size_t> &TensorArrayCPUWriteKernel::GetInputSizeList() const { return input_size_list_; }

const std::vector<size_t> &TensorArrayCPUWriteKernel::GetOutputSizeList() const { return output_size_list_; }

const std::vector<size_t> &TensorArrayCPUWriteKernel::GetWorkspaceSizeList() const { return workspace_size_list_; }

void TensorArrayCPUWriteKernel::InitKernel(const CNodePtr &kernel_node) {
  MS_EXCEPTION_IF_NULL(kernel_node);
  type_ = AnfAlgo::GetPrevNodeOutputInferDataType(kernel_node, kSecondInputIndex);
  shapes_ = AnfAlgo::GetInputDeviceShape(kernel_node, kSecondInputIndex);
  value_size_ = GetTypeByte(TypeIdToType(type_));
  for (auto i : shapes_) {
    value_size_ *= i;
  }
  input_size_list_.push_back(sizeof(int64_t));
  input_size_list_.push_back(sizeof(int64_t));
  input_size_list_.push_back(sizeof(value_size_));
  output_size_list_.push_back(sizeof(int64_t));
}

bool TensorArrayCPUWriteKernel::Launch(const std::vector<AddressPtr> &inputs, const std::vector<AddressPtr> &,
                                       const std::vector<AddressPtr> &) {
  auto handle_addr = GetDeviceAddress<int64_t>(inputs, 0);
  auto index = GetDeviceAddress<int64_t>(inputs, 1);
  auto value = GetDeviceAddress<unsigned char>(inputs, 2);
  MS_EXCEPTION_IF_NULL(handle_addr);
  MS_EXCEPTION_IF_NULL(index);
  MS_EXCEPTION_IF_NULL(value);
  int64_t index_host = index[0];
  CPUTensorArrayPtr tensors_ =
    std::dynamic_pointer_cast<CPUTensorArray>(TensorArrayMgr::GetInstance().GetTensorArray(handle_addr[0]));
  MS_EXCEPTION_IF_NULL(tensors_);
  if (!tensors_->CheckValue(type_, shapes_)) {
    MS_LOG(EXCEPTION) << "Invalid input data for tensor array write op.";
  }
  // Manage the value : create/reuse a device memory, and copy the input value to it.
  AddressPtr dev_addr = std::make_shared<kernel::Address>();
  MS_EXCEPTION_IF_NULL(dev_addr);
  if (tensors_->GetRealSize() > LongToSize(index_host)) {
    dev_addr->addr = tensors_->Read(index_host)->addr;
  } else {
    dev_addr->addr = luojianet_ms::device::cpu::CPUMemoryPool::GetInstance().AllocTensorMem(value_size_);
    MS_LOG(DEBUG) << "Create tensor " << dev_addr->addr << ", size " << value_size_;
  }
  MS_EXCEPTION_IF_NULL(dev_addr->addr);
  dev_addr->size = value_size_;
  auto ret = memcpy_s(dev_addr->addr, dev_addr->size, value, value_size_);
  if (ret != EOK) {
    MS_LOG(EXCEPTION) << "Memcpy failed, errorno(" << ret << ")";
  }
  if (tensors_->Write(index_host, dev_addr)) {
    MS_LOG(DEBUG) << "Write to tensorarry succeed, index " << index_host;
  } else {
    MS_LOG(EXCEPTION) << "Failed to write.";
  }
  return true;
}
}  // namespace kernel
}  // namespace luojianet_ms
