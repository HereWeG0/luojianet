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
#ifndef LUOJIANET_MS_LITE_SRC_RUNTIME_KERNEL_ARM_INT8_BATCH_TO_SPACE_INT8_H_
#define LUOJIANET_MS_LITE_SRC_RUNTIME_KERNEL_ARM_INT8_BATCH_TO_SPACE_INT8_H_

#include <vector>
#include "include/errorcode.h"
#include "nnacl/batch_to_space.h"
#include "nnacl/base/batch_to_space_base.h"
#include "nnacl/int8/batch_to_space_int8.h"
#include "src/inner_kernel.h"

namespace luojianet_ms::kernel {
class BatchToSpaceInt8CPUKernel : public InnerKernel {
 public:
  BatchToSpaceInt8CPUKernel(OpParameter *parameter, const std::vector<lite::Tensor *> &inputs,
                            const std::vector<lite::Tensor *> &outputs, const lite::InnerContext *ctx)
      : InnerKernel(parameter, inputs, outputs, ctx) {}

  ~BatchToSpaceInt8CPUKernel() override;

  int Prepare() override;
  int ReSize() override;
  int Run() override;

 private:
  QuantArg *in_quant_arg_ = nullptr;
  QuantArg *out_quant_arg_ = nullptr;
};
}  // namespace luojianet_ms::kernel

#endif  // LUOJIANET_MS_LITE_SRC_RUNTIME_KERNEL_ARM_INT8_BATCH_TO_SPACE_INT8_H_
