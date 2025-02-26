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
#ifndef LUOJIANET_MS_LITE_SRC_RUNTIME_KERNEL_ARM_BASE_CARRY_DATA_H_
#define LUOJIANET_MS_LITE_SRC_RUNTIME_KERNEL_ARM_BASE_CARRY_DATA_H_

#include <vector>
#include "src/inner_kernel.h"
#include "src/tensor.h"
#ifndef CONTROLFLOW_TENSORLIST_CLIP
#include "src/tensorlist.h"
#endif

namespace luojianet_ms::kernel {
class CarryDataKernel : public InnerKernel {
 public:
  CarryDataKernel(OpParameter *parameter, const std::vector<lite::Tensor *> &inputs,
                  const std::vector<lite::Tensor *> &outputs, const lite::InnerContext *ctx)
      : InnerKernel(parameter, inputs, outputs, ctx) {}
  ~CarryDataKernel() override = default;

 protected:
  int MoveData(const std::vector<lite::Tensor *>::iterator &dst_begin,
               const std::vector<lite::Tensor *>::iterator &dst_end,
               const std::vector<lite::Tensor *>::iterator &src_begin,
               const std::vector<lite::Tensor *>::iterator &src_limit);
  int MoveTensorData(lite::Tensor *dst_tensor, lite::Tensor *src_tensor);
#ifndef CONTROLFLOW_TENSORLIST_CLIP
  int MoveTensorListData(lite::TensorList *dst_tensorlist, lite::TensorList *src_tensorlist);
#endif
};
}  // namespace luojianet_ms::kernel

#endif  // LUOJIANET_MS_LITE_SRC_RUNTIME_KERNEL_ARM_BASE_CARRY_DATA_H_
