/**
 * Copyright 2021 Huawei Technologies Co., Ltd
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

#ifndef LUOJIANET_MS_LITE_SRC_RUNTIME_KERNEL_OPENCL_KERNEL_ARITHMETIC_H_
#define LUOJIANET_MS_LITE_SRC_RUNTIME_KERNEL_OPENCL_KERNEL_ARITHMETIC_H_

#include <vector>
#include <set>
#include <string>
#include "src/runtime/kernel/arm/fp32/arithmetic_fp32.h"
#include "src/runtime/kernel/opencl/opencl_kernel.h"

namespace luojianet_ms::kernel {
class ArithmeticInt8OpenCLKernel : public OpenCLKernel {
 public:
  using OpenCLKernel::OpenCLKernel;
  ~ArithmeticInt8OpenCLKernel() override = default;

  int Run() override;
  int Prepare() override;
  int CheckSpecs() override;
  int InitWeights() override;
  int SetConstArgs() override;
  void SetGlobalLocal() override;

 private:
  bool element_flag_{true};
  float activation_min_{-128};
  float activation_max_{127};
  GpuTensorInfo in0_shape_;
  GpuTensorInfo in1_shape_;
  GpuTensorInfo out_shape_;
  std::vector<void *> weight_ptrs_;
  std::string kernel_name_;
};
}  // namespace luojianet_ms::kernel

#endif  // LUOJIANET_MS_LITE_SRC_RUNTIME_KERNEL_OPENCL_KERNEL_ARITHMETIC_H_
