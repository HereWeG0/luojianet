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

#ifndef LUOJIANET_MS_LITE_SRC_RUNTIME_KERNEL_ARM_BASE_PRIOR_BOX_H_
#define LUOJIANET_MS_LITE_SRC_RUNTIME_KERNEL_ARM_BASE_PRIOR_BOX_H_

#include <vector>
#include "src/inner_kernel.h"
#include "nnacl/reshape_parameter.h"
#include "nnacl/fp32/prior_box_fp32.h"

using luojianet_ms::lite::InnerContext;

namespace luojianet_ms::kernel {
class PriorBoxCPUKernel : public InnerKernel {
 public:
  PriorBoxCPUKernel(OpParameter *parameter, const std::vector<lite::Tensor *> &inputs,
                    const std::vector<lite::Tensor *> &outputs, const InnerContext *ctx)
      : InnerKernel(parameter, inputs, outputs, ctx), ctx_(ctx), thread_count_(ctx->thread_num_) {
    prior_box_param_ = reinterpret_cast<PriorBoxParameter *>(op_parameter_);
  }
  ~PriorBoxCPUKernel() = default;

  int Prepare() override;
  int ReSize() override;
  int Run() override;
  int PriorBoxImpl(int task_id);

 protected:
  const InnerContext *ctx_;
  int thread_count_;

 private:
  std::vector<float> output_;
  PriorBoxParameter *prior_box_param_;
  int GeneratePriorBox();
};
}  // namespace luojianet_ms::kernel

#endif  // LUOJIANET_MS_LITE_SRC_RUNTIME_KERNEL_ARM_BASE_PRIOR_BOX_H_
