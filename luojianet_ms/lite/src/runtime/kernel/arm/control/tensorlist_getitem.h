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

#ifndef LUOJIANET_MS_LITE_SRC_RUNTIME_KERNEL_ARM_CONTROL_TENSORLISTGETITEM_H_
#define LUOJIANET_MS_LITE_SRC_RUNTIME_KERNEL_ARM_CONTROL_TENSORLISTGETITEM_H_

#include <vector>
#include "src/inner_kernel.h"
#include "src/tensorlist.h"
#include "schema/model_generated.h"
#include "nnacl/tensorlist_parameter.h"

namespace luojianet_ms::kernel {
class TensorListGetItemCPUKernel : public InnerKernel {
 public:
  TensorListGetItemCPUKernel(OpParameter *parameter, const std::vector<lite::Tensor *> &inputs,
                             const std::vector<lite::Tensor *> &outputs, const lite::InnerContext *ctx)
      : InnerKernel(parameter, inputs, outputs, ctx),
        dtype_(reinterpret_cast<TensorListParameter *>(parameter)->element_dtype_) {}
  ~TensorListGetItemCPUKernel() = default;

  int Prepare() override;
  int ReSize() override;
  int Run() override;

 private:
  int index_ = 0;
  int dtype_ = kTypeUnknown;
};
}  // namespace luojianet_ms::kernel

#endif  // LUOJIANET_MS_LITE_SRC_RUNTIME_KERNEL_ARM_CONTROL_TENSORLISTGETITEM_H_
