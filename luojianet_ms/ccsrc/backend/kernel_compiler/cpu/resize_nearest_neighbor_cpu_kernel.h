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

#ifndef LUOJIANET_MS_CCSRC_BACKEND_KERNEL_COMPILER_CPU_RESIZE_NEAREST_NEIGHBOR_CPU_KERNEL_H_
#define LUOJIANET_MS_CCSRC_BACKEND_KERNEL_COMPILER_CPU_RESIZE_NEAREST_NEIGHBOR_CPU_KERNEL_H_

#include <memory>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include "backend/kernel_compiler/cpu/cpu_kernel.h"
#include "backend/kernel_compiler/cpu/cpu_kernel_factory.h"

namespace luojianet_ms {
namespace kernel {
class ResizeNearestNeighborCPUKernel : public CPUKernel {
 public:
  ResizeNearestNeighborCPUKernel() = default;
  ~ResizeNearestNeighborCPUKernel() override = default;

  void InitKernel(const CNodePtr &kernel_node) override;

  bool Launch(const std::vector<AddressPtr> &inputs, const std::vector<AddressPtr> &workspace,
              const std::vector<AddressPtr> &outputs) override;

 private:
  template <typename T>
  void LaunchKernel(const std::vector<AddressPtr> &inputs, const std::vector<AddressPtr> &outputs);

  TypeId dtype_{kTypeUnknown};
  bool align_corners_{false};
  size_t batch_size_{0};
  size_t channel_{0};
  size_t in_height_{0};
  size_t in_width_{0};
  size_t out_height_{0};
  size_t out_width_{0};
  size_t output_size_{0};
  float height_scale_{1.0};
  float width_scale_{1.0};
};

MS_REG_CPU_KERNEL(ResizeNearestNeighbor,
                  KernelAttr().AddInputAttr(kNumberTypeFloat16).AddOutputAttr(kNumberTypeFloat16),
                  ResizeNearestNeighborCPUKernel);

MS_REG_CPU_KERNEL(ResizeNearestNeighbor,
                  KernelAttr().AddInputAttr(kNumberTypeFloat32).AddOutputAttr(kNumberTypeFloat32),
                  ResizeNearestNeighborCPUKernel);

MS_REG_CPU_KERNEL(ResizeNearestNeighbor,
                  KernelAttr().AddInputAttr(kNumberTypeFloat64).AddOutputAttr(kNumberTypeFloat64),
                  ResizeNearestNeighborCPUKernel);

MS_REG_CPU_KERNEL(ResizeNearestNeighbor, KernelAttr().AddInputAttr(kNumberTypeInt32).AddOutputAttr(kNumberTypeInt32),
                  ResizeNearestNeighborCPUKernel);

MS_REG_CPU_KERNEL(ResizeNearestNeighbor, KernelAttr().AddInputAttr(kNumberTypeInt64).AddOutputAttr(kNumberTypeInt64),
                  ResizeNearestNeighborCPUKernel);
}  // namespace kernel
}  // namespace luojianet_ms
#endif  // LUOJIANET_MS_CCSRC_BACKEND_KERNEL_COMPILER_CPU_RESIZE_NEAREST_NEIGHBOR_CPU_KERNEL_H_
