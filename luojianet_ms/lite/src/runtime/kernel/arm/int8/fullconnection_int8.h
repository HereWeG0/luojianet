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

#ifndef LUOJIANET_MS_LITE_SRC_RUNTIME_KERNEL_ARM_INT8_FULLCONNECTION_INT8_H_
#define LUOJIANET_MS_LITE_SRC_RUNTIME_KERNEL_ARM_INT8_FULLCONNECTION_INT8_H_

#include <vector>
#include "include/errorcode.h"
#include "src/runtime/kernel/arm/int8/matmul_base_int8.h"

namespace luojianet_ms::kernel {
class FullconnectionInt8CPUKernel : public MatmulBaseInt8CPUKernel {
 public:
  FullconnectionInt8CPUKernel(OpParameter *parameter, const std::vector<lite::Tensor *> &inputs,
                              const std::vector<lite::Tensor *> &outputs, const luojianet_ms::lite::InnerContext *ctx)
      : MatmulBaseInt8CPUKernel(parameter, inputs, outputs, ctx) {}
  ~FullconnectionInt8CPUKernel() override = default;
  int Prepare() override;
  int ReSize() override;
};
}  // namespace luojianet_ms::kernel

#endif  // LUOJIANET_MS_LITE_SRC_RUNTIME_KERNEL_ARM_INT8_FULLCONNECTION_INT8_H_
