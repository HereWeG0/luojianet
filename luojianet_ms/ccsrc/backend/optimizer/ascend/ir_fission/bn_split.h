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
#ifndef LUOJIANET_MS_CCSRC_BACKEND_OPTIMIZER_ASCEND_IR_FISSION_BN_SPLIT_H_
#define LUOJIANET_MS_CCSRC_BACKEND_OPTIMIZER_ASCEND_IR_FISSION_BN_SPLIT_H_

#include <string>
#include <vector>
#include "backend/optimizer/common/optimizer.h"
#include "backend/optimizer/common/helper.h"

namespace luojianet_ms {
namespace opt {
class BnSplit : public PatternProcessPass {
 public:
  explicit BnSplit(string name = "bn_split", bool multigraph = true) : PatternProcessPass(name, multigraph) {}
  ~BnSplit() override = default;
  const BaseRef DefinePattern() const override;
  const AnfNodePtr Process(const FuncGraphPtr &, const AnfNodePtr &, const EquivPtr &) const override;

 protected:
  bool CreateOutputsOfBNTrainingReduce(const FuncGraphPtr &graph, const CNodePtr &bn_cnode,
                                       std::vector<AnfNodePtr> *bn_training_reduce_outputs) const;
  AnfNodePtr CreateOutputsOfBNTrainingUpdate(const FuncGraphPtr &graph, const CNodePtr &bn_cnode,
                                             const std::vector<AnfNodePtr> &bn_training_reduce_outputs) const;

 private:
  AnfNodePtr SplitBatchNormForTBE(const FuncGraphPtr &func_graph, const AnfNodePtr &node) const;
};

class SyncBnSplit : public BnSplit {
 public:
  explicit SyncBnSplit(bool multigraph = true) : BnSplit("sync_bn_split", multigraph) {}
  ~SyncBnSplit() override = default;
  const BaseRef DefinePattern() const override;
  const AnfNodePtr Process(const FuncGraphPtr &, const AnfNodePtr &, const EquivPtr &) const override;

 private:
  AnfNodePtr SyncBNSplitForTBE(const FuncGraphPtr &func_graph, const AnfNodePtr &node) const;
};

AnfNodePtr CreateValueNodeOfDeviceNumReciprocal(const FuncGraphPtr &graph, const CNodePtr &sync_bn_cnode);

AnfNodePtr CreateAllReduceAndMul(const FuncGraphPtr &graph, const AnfNodePtr &allreduce_input,
                                 const CNodePtr &sync_bn_cnode, const PatternProcessPass &pass);
}  // namespace opt
}  // namespace luojianet_ms
#endif  // LUOJIANET_MS_CCSRC_BACKEND_OPTIMIZER_ASCEND_IR_FISSION_BN_SPLIT_H_
