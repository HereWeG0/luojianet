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

#include "backend/optimizer/gpu/bce_with_logits_loss_fusion.h"
#include <memory>
#include <vector>
#include <string>
#include "backend/session/anf_runtime_algorithm.h"
#include "ir/primitive.h"
#include "utils/utils.h"
#include "backend/optimizer/common/helper.h"
#include "runtime/device/gpu/kernel_info_setter.h"

namespace luojianet_ms {
namespace opt {
AnfNodePtr AddReduceNode(const FuncGraphPtr &func_graph, const AnfNodePtr &node) {
  MS_EXCEPTION_IF_NULL(func_graph);
  MS_EXCEPTION_IF_NULL(node);
  auto cnode = node->cast<CNodePtr>();
  MS_EXCEPTION_IF_NULL(cnode);
  std::vector<AnfNodePtr> node_inputs = {
    NewValueNode(std::make_shared<Primitive>(prim::kPrimBCEWithLogitsLoss->name()))};
  (void)node_inputs.insert(node_inputs.end(), cnode->inputs().begin() + 1, cnode->inputs().end());
  CNodePtr new_cnode = func_graph->NewCNode(node_inputs);
  MS_EXCEPTION_IF_NULL(new_cnode);
  auto predict_input = cnode->inputs()[1];
  auto new_node_dtype = {AnfAlgo::GetOutputInferDataType(predict_input, 0)};
  auto new_node_shape = {AnfAlgo::GetOutputInferShape(predict_input, 0)};
  AnfAlgo::SetOutputInferTypeAndShape(new_node_dtype, new_node_shape, new_cnode.get());

  // Add reduce node
  string reduction = AnfAlgo::GetNodeAttr<std::string>(node, kAttrReduction);
  MS_LOG(INFO) << "Create reduce node for BCEWithLogitsLoss, reduction attr is: " << reduction;
  std::vector<AnfNodePtr> reduce_inputs;
  if (reduction == "sum") {
    reduce_inputs = {NewValueNode(std::make_shared<Primitive>(prim::kPrimReduceSum->name())), new_cnode};
  } else if (reduction == "mean") {
    reduce_inputs = {NewValueNode(std::make_shared<Primitive>(prim::kPrimReduceMean->name())), new_cnode};
  } else {
    MS_LOG(INFO) << "Reduction is none, no optimization on current BCEWithLogitsLoss.";
    return nullptr;
  }
  auto reduce_node = func_graph->NewCNode(reduce_inputs);
  MS_EXCEPTION_IF_NULL(reduce_node);
  auto type = AnfAlgo::GetOutputInferDataType(node, 0);
  auto shape = {AnfAlgo::GetOutputInferShape(node, 0)};
  AnfAlgo::SetOutputInferTypeAndShape({type}, shape, reduce_node.get());
  AnfAlgo::SetNodeAttr(kAttrAxis, MakeValue(std::vector<int64_t>{}), reduce_node);
  AnfAlgo::SetNodeAttr("keep_dims", MakeValue(false), reduce_node);
  reduce_node->set_scope(cnode->scope());
  return reduce_node;
}

const BaseRef BCEWithLogitsLossFusion::DefinePattern() const {
  VarPtr Xs = std::make_shared<SeqVar>();
  MS_EXCEPTION_IF_NULL(Xs);
  return VectorRef({prim::kPrimBCEWithLogitsLoss, Xs});
}

const AnfNodePtr BCEWithLogitsLossFusion::Process(const FuncGraphPtr &func_graph, const AnfNodePtr &node,
                                                  const EquivPtr &) const {
  MS_EXCEPTION_IF_NULL(func_graph);
  MS_EXCEPTION_IF_NULL(node);
  auto cnode = node->cast<CNodePtr>();
  MS_EXCEPTION_IF_NULL(cnode);
  if (GetBoolAttr(cnode, kAttrVisited)) {
    return nullptr;
  }
  AnfAlgo::SetNodeAttr(kAttrVisited, MakeValue(true), node);
  if (cnode->inputs().size() == 0) {
    return nullptr;
  }
  if (!AnfAlgo::HasNodeAttr("reduction", cnode)) {
    MS_LOG(INFO) << "Primitive BCEWithLogitsLoss doesn't not have reduction attr.";
    return nullptr;
  }
  return AddReduceNode(func_graph, node);
}
}  // namespace opt
}  // namespace luojianet_ms
