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
#include "backend/optimizer/ascend/buffer_fusion/conv_double_in_fusion_pass.h"
#include "backend/kernel_compiler/kernel_fusion.h"
#include "backend/session/anf_runtime_algorithm.h"
#include "base/core_ops.h"
#include "utils/ms_context.h"
#include "backend/optimizer/common/fusion_id_allocator.h"

namespace luojianet_ms {
namespace opt {
void ConvDoubleInFusionPass::MatchConvDoubleInEltwise(const CNodePtr &cnode, const session::KernelGraph &kernel_graph,
                                                      FusedNodeRecord *candidate_fusion) {
  MS_EXCEPTION_IF_NULL(cnode);
  MS_EXCEPTION_IF_NULL(candidate_fusion);
  luojianet_ms::HashSet<AnfNodePtr> record{cnode};
  auto eltwise_input = cnode->input(kIndex1);
  MS_EXCEPTION_IF_NULL(eltwise_input);
  if (CheckDoubleInEltWiseNode(kernel_graph, eltwise_input)) {
    (void)record.insert(eltwise_input);
  } else {
    return;
  }
  auto input_cnode = eltwise_input->cast<CNodePtr>();
  MS_EXCEPTION_IF_NULL(input_cnode);
  auto double_in_eltwise_input = input_cnode->input(kIndex1);
  MS_EXCEPTION_IF_NULL(double_in_eltwise_input);
  if (!double_in_eltwise_input->isa<CNode>() || !AnfUtils::IsRealCNodeKernel(double_in_eltwise_input) ||
      fusion_id_allocator->HasFusionIdAttr(double_in_eltwise_input)) {
    return;
  }
  if (AnfAlgo::GetKernelType(double_in_eltwise_input) == KernelType::TBE_KERNEL &&
      AnfAlgo::GetFusionType(double_in_eltwise_input) == kernel::FusionType::CONV) {
    (void)record.insert(double_in_eltwise_input);
    candidate_fusion->push_back(record);
    SetRecordFusionId(record);
  }
}

void ConvDoubleInFusionPass::MatchSingleFusionPattern(const session::KernelGraph &kernel_graph,
                                                      FusedNodeRecord *candidate_fusion) {
  MS_EXCEPTION_IF_NULL(candidate_fusion);
  const auto &node_list = TopoSort(kernel_graph.get_return());
  for (auto &node : node_list) {
    if (!AnfUtils::IsRealCNodeKernel(node) || fusion_id_allocator->HasFusionIdAttr(node) ||
        AnfAlgo::CheckPrimitiveType(node, prim::kPrimReturn)) {
      continue;
    }
    auto cnode = node->cast<CNodePtr>();
    MS_EXCEPTION_IF_NULL(cnode);
    if (AnfAlgo::GetKernelType(cnode) == KernelType::TBE_KERNEL &&
        AnfAlgo::GetFusionType(cnode) == kernel::FusionType::ELEMWISE && cnode->inputs().size() == ELTWISE_INPUT_SIZE &&
        !AnfAlgo::CheckPrimitiveType(node, prim::kPrimReluV2)) {
      MatchConvDoubleInEltwise(cnode, kernel_graph, candidate_fusion);
    }
  }
}
}  // namespace opt
}  // namespace luojianet_ms
