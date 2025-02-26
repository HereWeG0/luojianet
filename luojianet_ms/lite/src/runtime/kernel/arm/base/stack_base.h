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
#ifndef LUOJIANET_MS_LITE_SRC_RUNTIME_KERNEL_ARM_BASE_STACK_BASE_H_
#define LUOJIANET_MS_LITE_SRC_RUNTIME_KERNEL_ARM_BASE_STACK_BASE_H_

#include <vector>
#include "src/inner_kernel.h"
#include "nnacl/stack_parameter.h"

using luojianet_ms::lite::InnerContext;
namespace luojianet_ms::kernel {
class StackBaseCPUKernel : public InnerKernel {
 public:
  StackBaseCPUKernel(OpParameter *parameter, const std::vector<lite::Tensor *> &inputs,
                     const std::vector<lite::Tensor *> &outputs, const InnerContext *ctx)
      : InnerKernel(parameter, inputs, outputs, ctx) {}
  ~StackBaseCPUKernel() override = default;

  int Prepare() override;
  int ReSize() override;
  int Run() override;
  int Execute(int task_id);

 protected:
  int axis_ = 0;
  size_t data_type_size_ = 0;
  size_t copy_size_ = 0;
  int outer_size_ = 1;
  int num_threads_ = 1;
  void **all_inputs_ = nullptr;
};
}  // namespace luojianet_ms::kernel
#endif  // LUOJIANET_MS_LITE_SRC_RUNTIME_KERNEL_ARM_BASE_STACK_BASE_H_
