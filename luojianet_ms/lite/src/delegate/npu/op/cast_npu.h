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

#ifndef LUOJIANET_MS_LITE_SRC_RUNTIME_DELEGATE_NPU_OP_CAST_NPU_H_
#define LUOJIANET_MS_LITE_SRC_RUNTIME_DELEGATE_NPU_OP_CAST_NPU_H_
#include <vector>
#include <string>
#include "include/graph/op/all_ops.h"
#include "src/delegate/npu/op/npu_op.h"

namespace luojianet_ms {
class CastNPUOp : public NPUOp {
 public:
  CastNPUOp(const schema::Primitive *primitive, const std::vector<luojianet_ms::MSTensor> &in_tensors,
            const std::vector<luojianet_ms::MSTensor> &out_tensors, std::string name)
      : NPUOp(primitive, in_tensors, out_tensors, name) {}

  ~CastNPUOp() override;

  int IsSupport(const schema::Primitive *primitive, const std::vector<luojianet_ms::MSTensor> &in_tensors,
                const std::vector<luojianet_ms::MSTensor> &out_tensors) override;

  int Init(const schema::Primitive *primitive, const std::vector<luojianet_ms::MSTensor> &in_tensors,
           const std::vector<luojianet_ms::MSTensor> &out_tensors) override;

  int SetNPUInputs(const std::vector<luojianet_ms::MSTensor> &in_tensors,
                   const std::vector<luojianet_ms::MSTensor> &out_tensors,
                   const std::vector<ge::Operator *> &npu_inputs) override;

  ge::Operator *GetNPUOp() override;

 private:
  hiai::op::CastT *cast_ = nullptr;
  int dst_type_;
};
}  // namespace luojianet_ms
#endif  // LUOJIANET_MS_LITE_SRC_RUNTIME_DELEGATE_NPU_OP_CAST_NPU_H_
