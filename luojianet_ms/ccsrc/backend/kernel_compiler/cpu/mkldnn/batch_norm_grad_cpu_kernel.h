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
#ifndef LUOJIANET_MS_CCSRC_BACKEND_KERNEL_COMPILER_CPU_BATCH_NORM_GRAD_CPU_KERNEL_H_
#define LUOJIANET_MS_CCSRC_BACKEND_KERNEL_COMPILER_CPU_BATCH_NORM_GRAD_CPU_KERNEL_H_
#include <memory>
#include <vector>
#include "backend/kernel_compiler/cpu/mkldnn/mkl_cpu_kernel.h"

namespace luojianet_ms {
namespace kernel {
constexpr size_t SCALE_SHIFT_NUM = 2;
constexpr size_t NC = 2;
constexpr size_t NCHW = 4;

class BatchNormGradCPUKernel : public MKLCPUKernel {
 public:
  BatchNormGradCPUKernel() = default;
  ~BatchNormGradCPUKernel() override = default;

  void InitKernel(const CNodePtr &kernel_node) override;

  bool Launch(const std::vector<AddressPtr> &inputs, const std::vector<AddressPtr> &workspace,
              const std::vector<AddressPtr> &outputs) override;

 protected:
  void InitInputOutputSize(const CNodePtr &kernel_node) override;

 private:
  float momentum{0.9};
  size_t batch_size{0};
  size_t channel{0};
  size_t hw_size{0};
  size_t nhw_size{0};
  enum format_ { N, C, H, W };
  enum input_list_ { Y_BACKPROP, X, SCALE, SAVE_MEAN, SAVE_VARIANCE, RESERVE };
  enum workspace_list_ { SCALE_BIAS, DIFF_SCALE_BIAS };
  enum output_list_ { DX, DSCALE, DBIAS };
};

MS_REG_CPU_KERNEL(BatchNormGrad,
                  KernelAttr()
                    .AddInputAttr(kNumberTypeFloat32)
                    .AddInputAttr(kNumberTypeFloat32)
                    .AddInputAttr(kNumberTypeFloat32)
                    .AddInputAttr(kNumberTypeFloat32)
                    .AddInputAttr(kNumberTypeFloat32)
                    .AddInputAttr(kNumberTypeFloat32)
                    .AddOutputAttr(kNumberTypeFloat32)
                    .AddOutputAttr(kNumberTypeFloat32)
                    .AddOutputAttr(kNumberTypeFloat32),
                  BatchNormGradCPUKernel)
}  // namespace kernel
}  // namespace luojianet_ms

#endif  // LUOJIANET_MS_CCSRC_BACKEND_KERNEL_COMPILER_CPU_FUSED_BATCH_NORM_GRAD_CPU_KERNEL_H_
