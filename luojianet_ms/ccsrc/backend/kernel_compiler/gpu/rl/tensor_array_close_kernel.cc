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

#include "backend/kernel_compiler/gpu/rl/tensor_array_close_kernel.h"
#include "backend/kernel_compiler/common_utils.h"
#include "runtime/device/gpu/gpu_tensor_array.h"
#include "runtime/device/tensor_array_manager.h"

namespace luojianet_ms {
namespace kernel {
using luojianet_ms::device::TensorArrayMgr;
using luojianet_ms::device::gpu::GPUTensorArray;
using luojianet_ms::device::gpu::GPUTensorArrayPtr;
TensorArrayCloseKernel::TensorArrayCloseKernel() {}

const std::vector<size_t> &TensorArrayCloseKernel::GetInputSizeList() const { return input_size_list_; }

const std::vector<size_t> &TensorArrayCloseKernel::GetOutputSizeList() const { return output_size_list_; }

const std::vector<size_t> &TensorArrayCloseKernel::GetWorkspaceSizeList() const { return workspace_size_list_; }

bool TensorArrayCloseKernel::Init(const CNodePtr &kernel_node) {
  MS_EXCEPTION_IF_NULL(kernel_node);
  InitSizeLists();
  return true;
}

void TensorArrayCloseKernel::InitSizeLists() {
  input_size_list_.push_back(sizeof(int64_t));
  output_size_list_.push_back(sizeof(int64_t));
}

bool TensorArrayCloseKernel::Launch(const std::vector<AddressPtr> &inputs, const std::vector<AddressPtr> &,
                                    const std::vector<AddressPtr> &, void *stream) {
  auto handle_addr = GetDeviceAddress<int64_t>(inputs, 0);
  MS_ERROR_IF_NULL(handle_addr);
  int64_t handle = 0;
  CHECK_CUDA_RET_WITH_EXCEPT(kernel_node_,
                             cudaMemcpyAsync(&handle, handle_addr, sizeof(int64_t), cudaMemcpyDeviceToHost,
                                             reinterpret_cast<cudaStream_t>(stream)),
                             "Get handle to host failed");
  GPUTensorArrayPtr tensors_ =
    std::dynamic_pointer_cast<GPUTensorArray>(TensorArrayMgr::GetInstance().GetTensorArray(handle));
  MS_ERROR_IF_NULL(tensors_);
  // Free device mem
  tensors_->Free();
  // Erase tensorarray
  if (!TensorArrayMgr::GetInstance().EraseTensorArray(handle)) {
    MS_LOG(EXCEPTION) << "Free tensorarray failed";
  }
  return true;
}
}  // namespace kernel
}  // namespace luojianet_ms
