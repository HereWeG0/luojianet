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

#ifndef LUOJIANET_MS_LITE_SRC_RUNTIME_KERNEL_ARM_FP32_SOFTMAX_H_
#define LUOJIANET_MS_LITE_SRC_RUNTIME_KERNEL_ARM_FP32_SOFTMAX_H_

#include <vector>
#include "src/inner_kernel.h"
#include "src/runtime/kernel/arm/base/softmax_base.h"

namespace luojianet_ms::kernel {
class SoftmaxCPUKernel : public SoftmaxBaseCPUKernel {
 public:
  SoftmaxCPUKernel(OpParameter *parameter, const std::vector<lite::Tensor *> &inputs,
                   const std::vector<lite::Tensor *> &outputs, const lite::InnerContext *ctx)
      : SoftmaxBaseCPUKernel(parameter, inputs, outputs, ctx), sum_data_(nullptr) {}
  ~SoftmaxCPUKernel() override {
    if (sum_data_ != nullptr) {
      free(sum_data_);
    }
  };

  int Prepare() override;
  int ReSize() override;
  int Run() override;
  int DoSoftmaxLastAxis(int task_id);

 private:
  float *sum_data_ = nullptr;
  int in_plane_size_ = 0;
  int out_plane_size_ = 0;
};
}  // namespace luojianet_ms::kernel

#endif  // LUOJIANET_MS_LITE_SRC_RUNTIME_KERNEL_ARM_FP32_SOFTMAX_H_
