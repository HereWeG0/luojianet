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

#ifndef LUOJIANET_MS_LITE_SRC_RUNTIME_KERNEL_ARM_INT8_CROP_INT8_H_
#define LUOJIANET_MS_LITE_SRC_RUNTIME_KERNEL_ARM_INT8_CROP_INT8_H_

#include <vector>
#include <limits>
#include "include/errorcode.h"
#include "include/context.h"
#include "nnacl/int8/crop_int8.h"
#include "src/inner_kernel.h"
#include "src/runtime/kernel/arm/base/crop_base.h"

namespace luojianet_ms::kernel {
class CropInt8CPUKernel : public CropBaseCPUKernel {
 public:
  CropInt8CPUKernel(OpParameter *parameter, const std::vector<lite::Tensor *> &inputs,
                    const std::vector<lite::Tensor *> &outputs, const luojianet_ms::lite::InnerContext *ctx)
      : CropBaseCPUKernel(parameter, inputs, outputs, ctx) {}
  ~CropInt8CPUKernel() = default;

  int Prepare() override;
  int ReSize() override;
  int Run() override;
  void DoExecute(int task_id);
};

int CropInt8Run(void *cdata, int task_id, float lhs_scale, float rhs_scale);
}  // namespace luojianet_ms::kernel

#endif  // LUOJIANET_MS_LITE_SRC_RUNTIME_KERNEL_ARM_INT8_CROP_INT8_H_
