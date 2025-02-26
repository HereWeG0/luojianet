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
#include "backend/optimizer/ascend/buffer_fusion/depthwiseconv_eltwise_fusion_pass.h"

#include "backend/kernel_compiler/kernel_fusion.h"
#include "backend/session/anf_runtime_algorithm.h"
#include "base/core_ops.h"
#include "utils/ms_context.h"
#include "backend/optimizer/common/fusion_id_allocator.h"

namespace luojianet_ms {
namespace opt {
void DepthwiseConvEltwiseFusionPass::MatchDepthwiseConvRelu(const CNodePtr &cnode,
                                                            const session::KernelGraph & /* kernel_graph */,
                                                            FusedNodeRecord *candidate_fusion, bool is_order) {
  MS_EXCEPTION_IF_NULL(cnode);
  MS_EXCEPTION_IF_NULL(candidate_fusion);
  if (is_order) {
    // DepthwiseConvolution--->Elemwise
    auto depthwise_conv = cnode->input(kIndex1);
    MS_EXCEPTION_IF_NULL(depthwise_conv);
    if (cnode->isa<CNode>() && IsPrimitiveCNode(depthwise_conv, prim::kPrimDepthwiseConv2dNative)) {
      luojianet_ms::HashSet<AnfNodePtr> record{cnode, depthwise_conv};
      candidate_fusion->push_back(record);
      SetRecordFusionId(record);
    }
  } else {
    // Elemwise-->DepthwiseConvolution
    auto relu = cnode->input(kIndex1);
    MS_EXCEPTION_IF_NULL(relu);
    if (cnode->isa<CNode>() && (IsPrimitiveCNode(relu, prim::kPrimRelu) || IsPrimitiveCNode(relu, prim::kPrimReluV2))) {
      luojianet_ms::HashSet<AnfNodePtr> record{cnode, relu};
      candidate_fusion->push_back(record);
      SetRecordFusionId(record);
    }
  }
}

void DepthwiseConvEltwiseFusionPass::MatchSingleFusionPattern(const session::KernelGraph &kernel_graph,
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
        AnfAlgo::GetFusionType(cnode) == kernel::FusionType::ELEMWISE) {
      auto eltwise_input = cnode->input(kIndex1);
      if (eltwise_input->isa<CNode>() && AnfAlgo::CheckPrimitiveType(eltwise_input, prim::kPrimDepthwiseConv2dNative)) {
        MatchDepthwiseConvRelu(cnode, kernel_graph, candidate_fusion, true);
      }
    } else if (AnfAlgo::GetCNodeName(cnode) == prim::kPrimDepthwiseConv2dNative->name()) {
      MatchDepthwiseConvRelu(cnode, kernel_graph, candidate_fusion, false);
    }
  }
}
}  // namespace opt
}  // namespace luojianet_ms
