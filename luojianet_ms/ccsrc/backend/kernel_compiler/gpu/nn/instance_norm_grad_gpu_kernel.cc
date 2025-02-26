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

#include "backend/kernel_compiler/gpu/nn/instance_norm_grad_gpu_kernel.h"

namespace luojianet_ms {
namespace kernel {
MS_REG_GPU_KERNEL_ONE(InstanceNormGrad,
                      KernelAttr()
                        .AddInputAttr(kNumberTypeFloat32)    // dy
                        .AddInputAttr(kNumberTypeFloat32)    // x
                        .AddInputAttr(kNumberTypeFloat32)    // scale
                        .AddInputAttr(kNumberTypeFloat32)    // save_mean
                        .AddInputAttr(kNumberTypeFloat32)    // save_variance
                        .AddOutputAttr(kNumberTypeFloat32)   // dx
                        .AddOutputAttr(kNumberTypeFloat32)   // dscale
                        .AddOutputAttr(kNumberTypeFloat32),  // dbias
                      InstanceNormGradGpuKernel, float)
MS_REG_GPU_KERNEL_ONE(InstanceNormGrad,
                      KernelAttr()
                        .AddInputAttr(kNumberTypeFloat16)    // dy
                        .AddInputAttr(kNumberTypeFloat16)    // x
                        .AddInputAttr(kNumberTypeFloat32)    // scale
                        .AddInputAttr(kNumberTypeFloat32)    // save_mean
                        .AddInputAttr(kNumberTypeFloat32)    // save_variance
                        .AddOutputAttr(kNumberTypeFloat16)   // dx
                        .AddOutputAttr(kNumberTypeFloat32)   // dscale
                        .AddOutputAttr(kNumberTypeFloat32),  // dbias
                      InstanceNormGradGpuKernel, half)
}  // namespace kernel
}  // namespace luojianet_ms
