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
#ifndef LUOJIANET_MS_LITE_SRC_RUNTIME_KERNEL_ARM_FP32_INSTANCE_NORM_H_
#define LUOJIANET_MS_LITE_SRC_RUNTIME_KERNEL_ARM_FP32_INSTANCE_NORM_H_
#include <vector>
#include "src/inner_kernel.h"
#include "include/context.h"
#include "nnacl/instance_norm_parameter.h"

using luojianet_ms::lite::InnerContext;

namespace luojianet_ms::kernel {
class InstanceNormCPUKernel : public InnerKernel {
 public:
  InstanceNormCPUKernel(OpParameter *parameter, const std::vector<lite::Tensor *> &inputs,
                        const std::vector<lite::Tensor *> &outputs, const lite::InnerContext *ctx)
      : InnerKernel(parameter, inputs, outputs, ctx) {
    param_ = reinterpret_cast<InstanceNormParameter *>(parameter);
  }
  ~InstanceNormCPUKernel() override{};

  int Prepare() override;
  int ReSize() override;
  int Run() override;
  int DoInstanceNorm(int task_id) const;

 private:
  void FreeTmpBuffer() {
    if (tmp_src_data_ != nullptr) {
      ms_context_->allocator->Free(tmp_src_data_);
      tmp_src_data_ = nullptr;
    }
  }

 private:
  InstanceNormParameter *param_ = nullptr;
  float *src_data_ = nullptr;
  float *tmp_src_data_ = nullptr;
  float *dst_data_ = nullptr;
  float *gamma_data_ = nullptr;
  float *beta_data_ = nullptr;
};
}  // namespace luojianet_ms::kernel

#endif  // LUOJIANET_MS_LITE_SRC_RUNTIME_KERNEL_ARM_FP32_INSTANCE_NORM_H_
