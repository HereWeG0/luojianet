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

#ifndef LUOJIANET_MS_LITE_SRC_RUNTIME_KERNEL_ARM_FP32_GATHER_H_
#define LUOJIANET_MS_LITE_SRC_RUNTIME_KERNEL_ARM_FP32_GATHER_H_

#include <vector>
#include "include/errorcode.h"
#include "src/inner_kernel.h"
#include "nnacl/gather_parameter.h"
#include "nnacl/base/gather_base.h"

namespace luojianet_ms::kernel {
class GatherCPUKernel : public InnerKernel {
 public:
  GatherCPUKernel(OpParameter *parameter, const std::vector<lite::Tensor *> &inputs,
                  const std::vector<lite::Tensor *> &outputs, const lite::InnerContext *ctx)
      : InnerKernel(parameter, inputs, outputs, ctx) {}
  ~GatherCPUKernel() = default;

  int Prepare() override;
  int ReSize() override;
  int Run() override;
  int DoGather(int task_id) const;

 private:
  int *indices_data_ = nullptr;
  int axis_ = 0;
  int AssignIndicesData(bool isIndicesInt32, int indices_num, lite::Tensor *indices_tensor);
};
}  // namespace luojianet_ms::kernel

#endif  // LUOJIANET_MS_LITE_SRC_RUNTIME_KERNEL_ARM_FP32_GATHER_H_
