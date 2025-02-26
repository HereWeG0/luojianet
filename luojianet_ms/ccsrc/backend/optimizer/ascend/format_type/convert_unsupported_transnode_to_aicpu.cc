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

#include "backend/optimizer/ascend/format_type/convert_unsupported_transnode_to_aicpu.h"
#include <memory>
#include "backend/session/anf_runtime_algorithm.h"
#include "backend/kernel_compiler/kernel_build_info.h"

namespace luojianet_ms {
namespace opt {
const BaseRef ConvertUnSupportNodeToAICPU::DefinePattern() const {
  VarPtr X = std::make_shared<Var>();
  VarPtr Xs = std::make_shared<SeqVar>();
  return VectorRef({X, Xs});
}

const AnfNodePtr ConvertUnSupportNodeToAICPU::Process(const luojianet_ms::FuncGraphPtr &,
                                                      const luojianet_ms::AnfNodePtr &node,
                                                      const luojianet_ms::EquivPtr &) const {
  if (node == nullptr || !node->isa<CNode>()) {
    return nullptr;
  }
  auto node_name = AnfAlgo::GetCNodeName(node);
  if (node_name != prim::kPrimTransData->name() && node_name != prim::kPrimCast->name()) {
    return nullptr;
  }
  auto kernel_builder_info = AnfAlgo::GetSelectKernelBuildInfo(node);
  if (supported_checker_->CheckAICoreSupported(node, kernel_builder_info)) {
    return nullptr;
  } else if (supported_checker_->CheckAICPUSupported(node, kernel_builder_info)) {
    auto builder = std::make_shared<kernel::KernelBuildInfo::KernelBuildInfoBuilder>(kernel_builder_info);
    MS_EXCEPTION_IF_NULL(builder);
    builder->SetKernelType(AICPU_KERNEL);
    AnfAlgo::SetSelectKernelBuildInfo(builder->Build(), node.get());
    AnfAlgo::SetNodeAttr(kAttrIsAICPUKernel, MakeValue(true), node);
  } else {
    MS_LOG(EXCEPTION) << "Kernel " << kernel_builder_info->ToString() << "is not supported in AiCPU & AiCore : node ["
                      << node->DebugString() << "]" << trace::DumpSourceLines(node);
  }
  return nullptr;
}
}  // namespace opt
}  // namespace luojianet_ms
