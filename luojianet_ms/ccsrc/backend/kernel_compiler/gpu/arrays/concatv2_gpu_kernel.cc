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

#include "backend/kernel_compiler/gpu/arrays/concatv2_gpu_kernel.h"

namespace luojianet_ms {
namespace kernel {
MS_REG_GPU_KERNEL_ONE(
  Concat, KernelAttr().AddAllSameAttr(true).AddInputAttr(kNumberTypeFloat64).AddOutputAttr(kNumberTypeFloat64),
  ConcatV2GpuFwdKernel, double)
MS_REG_GPU_KERNEL_ONE(
  Concat, KernelAttr().AddAllSameAttr(true).AddInputAttr(kNumberTypeFloat32).AddOutputAttr(kNumberTypeFloat32),
  ConcatV2GpuFwdKernel, float)
MS_REG_GPU_KERNEL_ONE(
  Concat, KernelAttr().AddAllSameAttr(true).AddInputAttr(kNumberTypeFloat16).AddOutputAttr(kNumberTypeFloat16),
  ConcatV2GpuFwdKernel, half)

MS_REG_GPU_KERNEL_ONE(Concat,
                      KernelAttr().AddAllSameAttr(true).AddInputAttr(kNumberTypeInt64).AddOutputAttr(kNumberTypeInt64),
                      ConcatV2GpuFwdKernel, int64_t)
MS_REG_GPU_KERNEL_ONE(Concat,
                      KernelAttr().AddAllSameAttr(true).AddInputAttr(kNumberTypeInt32).AddOutputAttr(kNumberTypeInt32),
                      ConcatV2GpuFwdKernel, int)
MS_REG_GPU_KERNEL_ONE(Concat,
                      KernelAttr().AddAllSameAttr(true).AddInputAttr(kNumberTypeInt16).AddOutputAttr(kNumberTypeInt16),
                      ConcatV2GpuFwdKernel, short)  // NOLINT
MS_REG_GPU_KERNEL_ONE(Concat,
                      KernelAttr().AddAllSameAttr(true).AddInputAttr(kNumberTypeInt8).AddOutputAttr(kNumberTypeInt8),
                      ConcatV2GpuFwdKernel, char)

MS_REG_GPU_KERNEL_ONE(
  Concat, KernelAttr().AddAllSameAttr(true).AddInputAttr(kNumberTypeUInt64).AddOutputAttr(kNumberTypeUInt64),
  ConcatV2GpuFwdKernel, uint64_t)
MS_REG_GPU_KERNEL_ONE(
  Concat, KernelAttr().AddAllSameAttr(true).AddInputAttr(kNumberTypeUInt32).AddOutputAttr(kNumberTypeUInt32),
  ConcatV2GpuFwdKernel, uint)
MS_REG_GPU_KERNEL_ONE(
  Concat, KernelAttr().AddAllSameAttr(true).AddInputAttr(kNumberTypeUInt16).AddOutputAttr(kNumberTypeUInt16),
  ConcatV2GpuFwdKernel, uint16_t)
MS_REG_GPU_KERNEL_ONE(Concat,
                      KernelAttr().AddAllSameAttr(true).AddInputAttr(kNumberTypeUInt8).AddOutputAttr(kNumberTypeUInt8),
                      ConcatV2GpuFwdKernel, uchar)

MS_REG_GPU_KERNEL_ONE(Concat,
                      KernelAttr().AddAllSameAttr(true).AddInputAttr(kNumberTypeBool).AddOutputAttr(kNumberTypeBool),
                      ConcatV2GpuFwdKernel, bool)
}  // namespace kernel
}  // namespace luojianet_ms
