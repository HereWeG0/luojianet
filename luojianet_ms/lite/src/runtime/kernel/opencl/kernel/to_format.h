/**
 * Copyright 2020 Huawei Technologies Co., Ltd
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

#ifndef LUOJIANET_MS_LITE_SRC_RUNTIME_KERNEL_OPENCL_KERNEL_TO_FORMAT_H_
#define LUOJIANET_MS_LITE_SRC_RUNTIME_KERNEL_OPENCL_KERNEL_TO_FORMAT_H_

#include <vector>
#include "src/inner_kernel.h"
#include "src/runtime/kernel/opencl/opencl_kernel.h"

namespace luojianet_ms::kernel {
class ToFormatOpenCLKernel : public OpenCLKernel {
 public:
  ToFormatOpenCLKernel(OpParameter *parameter, const std::vector<lite::Tensor *> &inputs,
                       const std::vector<lite::Tensor *> &outputs, const lite::InnerContext *ctx)
      : OpenCLKernel(parameter, inputs, outputs, ctx) {
    out_mem_type_ = reinterpret_cast<OpenCLToFormatParameter *>(op_parameter_)->out_mem_type;
  }
  ~ToFormatOpenCLKernel() override = default;

  int Run() override;
  int Prepare() override;

  int CheckSpecs() override;
  int SetConstArgs() override;
  void SetGlobalLocal() override;
  int InferShape() override;

 private:
  size_t N_{1};
  size_t H_{1};
  size_t W_{1};
  size_t C_{1};
};
}  // namespace luojianet_ms::kernel

#endif  // LUOJIANET_MS_LITE_SRC_RUNTIME_KERNEL_OPENCL_KERNEL_RESHAPE_H_
