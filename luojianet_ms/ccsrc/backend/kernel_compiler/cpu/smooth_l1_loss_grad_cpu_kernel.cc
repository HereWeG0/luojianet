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

#include "backend/kernel_compiler/cpu/smooth_l1_loss_grad_cpu_kernel.h"
#include "runtime/device/cpu/cpu_device_address.h"

namespace luojianet_ms {
namespace kernel {
namespace {
constexpr size_t kSmoothL1LossGradInputsNum = 3;
constexpr size_t kSmoothL1LossGradOutputsNum = 1;
}  // namespace

template <typename T>
void SmoothL1LossGradCPUKernel<T>::InitKernel(const CNodePtr &kernel_node) {
  MS_EXCEPTION_IF_NULL(kernel_node);
  kernel_name_ = AnfAlgo::GetCNodeName(kernel_node);
  beta_ = AnfAlgo::GetNodeAttr<float>(kernel_node, "beta");
  if (beta_ == 0.0) {
    MS_LOG(EXCEPTION) << "For '" << kernel_name_ << ", the 'beta' should not be 0.";
  }
  std::vector<size_t> x_shape = AnfAlgo::GetPrevNodeOutputInferShape(kernel_node, 0);
  for (const uint64_t &d : x_shape) {
    tensor_size_ *= d;
  }
}

template <typename T>
bool SmoothL1LossGradCPUKernel<T>::Launch(const std::vector<kernel::AddressPtr> &inputs,
                                          const std::vector<kernel::AddressPtr> &,
                                          const std::vector<kernel::AddressPtr> &outputs) {
  CHECK_KERNEL_INPUTS_NUM(inputs.size(), kSmoothL1LossGradInputsNum, kernel_name_);
  CHECK_KERNEL_OUTPUTS_NUM(outputs.size(), kSmoothL1LossGradOutputsNum, kernel_name_);
  const auto *predict_addr = reinterpret_cast<T *>(inputs[0]->addr);
  const auto *target_addr = reinterpret_cast<T *>(inputs[1]->addr);
  const auto *dloss_addr = reinterpret_cast<T *>(inputs[2]->addr);
  auto *result_addr = reinterpret_cast<T *>(outputs[0]->addr);
  T beta = (T)beta_;
  for (uint64_t i = 0; i < tensor_size_; ++i) {
    T diff = predict_addr[i] - target_addr[i];
    if (diff > beta) {
      result_addr[i] = dloss_addr[i];
    } else if (diff < -beta) {
      result_addr[i] = -dloss_addr[i];
    } else {
      result_addr[i] = (diff / beta) * dloss_addr[i];
    }
  }
  return true;
}
}  // namespace kernel
}  // namespace luojianet_ms
