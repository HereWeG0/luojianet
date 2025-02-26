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
#ifndef LUOJIANET_MS_CCSRC_BACKEND_OPTIMIZER_ASCEND_ENHANCER_INSERT_TENSOR_MOVE_FOR_HCCL_OP_H_
#define LUOJIANET_MS_CCSRC_BACKEND_OPTIMIZER_ASCEND_ENHANCER_INSERT_TENSOR_MOVE_FOR_HCCL_OP_H_

#include <memory>
#include "backend/optimizer/common/optimizer.h"
#include "backend/optimizer/ascend/ascend_helper.h"

namespace luojianet_ms {
namespace opt {
class InsertTensorMoveForHcclOp : public PatternProcessPass {
 public:
  explicit InsertTensorMoveForHcclOp(bool multigraph = true)
      : PatternProcessPass("insert_tensor_move_for_hccl_op", multigraph),
        kernel_query_(std::make_shared<KernelQuery>()) {}
  ~InsertTensorMoveForHcclOp() override = default;
  const AnfNodePtr Process(const FuncGraphPtr &, const AnfNodePtr &, const EquivPtr &) const override;

 private:
  void InsertTensorMove(const FuncGraphPtr &graph, const CNodePtr &hccl_node) const;
  bool NeedInsertTensorMove(const FuncGraphPtr &graph, const AnfNodePtr &input, size_t input_idx,
                            const CNodePtr &cur_node) const;
  KernelQueryPtr kernel_query_;
};
}  // namespace opt
}  // namespace luojianet_ms
#endif  // LUOJIANET_MS_CCSRC_BACKEND_OPTIMIZER_ASCEND_ENHANCER_INSERT_TENSOR_MOVE_FOR_HCCL_OP_H_
