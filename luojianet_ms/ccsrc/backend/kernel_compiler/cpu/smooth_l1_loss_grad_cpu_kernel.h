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

#ifndef LUOJIANET_MS_CCSRC_BACKEND_KERNEL_COMPILER_CPU_SMOOTH_L1_LOSS_GRAD_CPU_KERNEL_H_
#define LUOJIANET_MS_CCSRC_BACKEND_KERNEL_COMPILER_CPU_SMOOTH_L1_LOSS_GRAD_CPU_KERNEL_H_

#include <memory>
#include <unordered_map>
#include <vector>
#include "backend/kernel_compiler/cpu/cpu_kernel.h"
#include "backend/kernel_compiler/cpu/cpu_kernel_factory.h"

namespace luojianet_ms {
namespace kernel {
template <typename T>
class SmoothL1LossGradCPUKernel : public CPUKernel {
 public:
  SmoothL1LossGradCPUKernel() = default;
  ~SmoothL1LossGradCPUKernel() override = default;

  void InitKernel(const CNodePtr &kernel_node) override;

  bool Launch(const std::vector<AddressPtr> &inputs, const std::vector<AddressPtr> &workspace,
              const std::vector<AddressPtr> &outputs) override;

 private:
  float beta_{1.0};
  uint64_t tensor_size_{1};
};

MS_REG_CPU_KERNEL_T(SmoothL1LossGrad,
                    KernelAttr()
                      .AddInputAttr(kNumberTypeFloat16)
                      .AddInputAttr(kNumberTypeFloat16)
                      .AddInputAttr(kNumberTypeFloat16)
                      .AddOutputAttr(kNumberTypeFloat16),
                    SmoothL1LossGradCPUKernel, float16);

MS_REG_CPU_KERNEL_T(SmoothL1LossGrad,
                    KernelAttr()
                      .AddInputAttr(kNumberTypeFloat32)
                      .AddInputAttr(kNumberTypeFloat32)
                      .AddInputAttr(kNumberTypeFloat32)
                      .AddOutputAttr(kNumberTypeFloat32),
                    SmoothL1LossGradCPUKernel, float);
}  // namespace kernel
}  // namespace luojianet_ms
#endif  // LUOJIANET_MS_CCSRC_BACKEND_KERNEL_COMPILER_CPU_SMOOTH_L1_LOSS_GRAD_CPU_KERNEL_H_
