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

#ifndef LUOJIANET_MS_LITE_SRC_RUNTIME_KERNEL_ARM_INT8_SPLIT_INT8_H_
#define LUOJIANET_MS_LITE_SRC_RUNTIME_KERNEL_ARM_INT8_SPLIT_INT8_H_

#include <vector>
#include "src/inner_kernel.h"
#include "include/context.h"
#include "src/runtime/kernel/arm/base/split_base.h"

using luojianet_ms::lite::InnerContext;

namespace luojianet_ms::kernel {
class SplitInt8CPUKernel : public SplitBaseCPUKernel {
 public:
  SplitInt8CPUKernel(OpParameter *parameter, const std::vector<lite::Tensor *> &inputs,
                     const std::vector<lite::Tensor *> &outputs, const InnerContext *ctx)
      : SplitBaseCPUKernel(parameter, inputs, outputs, ctx) {}
  ~SplitInt8CPUKernel() = default;

  int Prepare() override;
  int ReSize() override;
  int Run() override;
  int Split(int task_id);

 private:
  int8_t *input_ptr_{nullptr};
  std::vector<int8_t *> output_ptr_;
};
}  // namespace luojianet_ms::kernel

#endif  // LUOJIANET_MS_LITE_SRC_RUNTIME_KERNEL_ARM_INT8_SPLIT_INT8_H_
