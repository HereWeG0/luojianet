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

#include "backend/kernel_compiler/gpu/nn/fused_add_relu_grad_v2_gpu_kernel.h"

namespace luojianet_ms {
namespace kernel {
MS_REG_GPU_KERNEL_ONE(FusedAddReluGradV2,
                      KernelAttr()
                        .AddInputAttr(kNumberTypeFloat32)
                        .AddInputAttr(kNumberTypeFloat32)
                        .AddInputAttr(kNumberTypeUInt32)
                        .AddOutputAttr(kNumberTypeFloat32),
                      FusedAddReluGradV2GpuKernel, float)
MS_REG_GPU_KERNEL_ONE(FusedAddReluGradV2,
                      KernelAttr()
                        .AddInputAttr(kNumberTypeFloat16)
                        .AddInputAttr(kNumberTypeFloat16)
                        .AddInputAttr(kNumberTypeUInt32)
                        .AddOutputAttr(kNumberTypeFloat16),
                      FusedAddReluGradV2GpuKernel, half)
MS_REG_GPU_KERNEL_ONE(FusedAddReluGradV2,
                      KernelAttr()
                        .AddInputAttr(kNumberTypeInt32)
                        .AddInputAttr(kNumberTypeInt32)
                        .AddInputAttr(kNumberTypeUInt32)
                        .AddOutputAttr(kNumberTypeInt32),
                      FusedAddReluGradV2GpuKernel, int32_t)
MS_REG_GPU_KERNEL_ONE(FusedAddReluGradV2,
                      KernelAttr()
                        .AddInputAttr(kNumberTypeInt64)
                        .AddInputAttr(kNumberTypeInt64)
                        .AddInputAttr(kNumberTypeUInt32)
                        .AddOutputAttr(kNumberTypeInt64),
                      FusedAddReluGradV2GpuKernel, int64_t)
}  // namespace kernel
}  // namespace luojianet_ms
