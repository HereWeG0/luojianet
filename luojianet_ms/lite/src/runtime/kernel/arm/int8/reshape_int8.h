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

#ifndef LUOJIANET_MS_LITE_SRC_RUNTIME_KERNEL_ARM_INT8_RESHAPE_INT8_H_
#define LUOJIANET_MS_LITE_SRC_RUNTIME_KERNEL_ARM_INT8_RESHAPE_INT8_H_

#include <vector>
#include "src/inner_kernel.h"
#include "include/context.h"
#include "nnacl/reshape_parameter.h"

using luojianet_ms::lite::InnerContext;

namespace luojianet_ms::kernel {
class ReshapeInt8CPUKernel : public InnerKernel {
 public:
  ReshapeInt8CPUKernel(OpParameter *parameter, const std::vector<lite::Tensor *> &inputs,
                       const std::vector<lite::Tensor *> &outputs, const InnerContext *ctx)
      : InnerKernel(parameter, inputs, outputs, ctx) {
    reshape_param_ = reinterpret_cast<ReshapeParameter *>(op_parameter_);
  }
  ~ReshapeInt8CPUKernel() = default;

  int Prepare() override;
  int ReSize() override;
  int Run() override;
  int DoExecute(int task_id);

 private:
  int64_t elements_num_ = 0;
  int64_t count_unit_ = 0;
  int8_t *input_data_ = nullptr;
  int8_t *output_data_ = nullptr;
  ReshapeParameter *reshape_param_ = nullptr;
};

int ReshapeInt8Run(void *cdata, int task_id, float lhs_scale, float rhs_scale);
}  // namespace luojianet_ms::kernel

#endif  // LUOJIANET_MS_LITE_SRC_RUNTIME_KERNEL_ARM_INT8_RESHAPE_INT8_H_
