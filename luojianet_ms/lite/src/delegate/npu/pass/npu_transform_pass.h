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

#ifndef LUOJIANET_MS_LITE_SRC_RUNTIME_DELEGATE_NPU_PASS_NPU_TRANSFORM_PASS_H_
#define LUOJIANET_MS_LITE_SRC_RUNTIME_DELEGATE_NPU_PASS_NPU_TRANSFORM_PASS_H_

#include <set>
#include <vector>
#include "src/delegate/npu/op/npu_op.h"
#include "src/delegate/npu/pass/npu_base_pass.h"

namespace luojianet_ms {
class NPUTransformPass : public NPUBasePass {
 public:
  NPUTransformPass() { name_ = "NPUTransformPass"; }

  int Run(NPUGraph *subgraph) override;

 private:
  int InsertPreNodes(NPUOp *op, std::vector<NPUOp *> *trans_ops);

  int InsertPostNodes(NPUOp *op, std::vector<NPUOp *> *trans_ops, std::vector<luojianet_ms::MSTensor> graph_outputs);

 private:
  int total = 0;
  std::vector<NPUOp *> *all_ops_;
  std::vector<luojianet_ms::MSTensor *> *all_tensors_;
};
}  // namespace luojianet_ms
#endif  // LUOJIANET_MS_LITE_SRC_RUNTIME_DELEGATE_NPU_PASS_NPU_TRANSFORM_PASS_H_
