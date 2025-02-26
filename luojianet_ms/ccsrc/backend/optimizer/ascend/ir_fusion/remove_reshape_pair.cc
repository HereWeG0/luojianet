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

#include "backend/optimizer/ascend/ir_fusion/remove_reshape_pair.h"
#include <memory>
#include "backend/session/anf_runtime_algorithm.h"
#include "backend/kernel_compiler/common_utils.h"

namespace luojianet_ms {
namespace opt {
const BaseRef RemoveReshapePair::DefinePattern() const {
  VarPtr X = std::make_shared<Var>();
  MS_EXCEPTION_IF_NULL(X);
  return VectorRef({prim::kPrimReshape, VectorRef({prim::kPrimReshape, X})});
}

const AnfNodePtr RemoveReshapePair::Process(const FuncGraphPtr &func_graph, const AnfNodePtr &node,
                                            const EquivPtr &equiv) const {
  MS_EXCEPTION_IF_NULL(func_graph);
  MS_EXCEPTION_IF_NULL(equiv);
  auto out_reshape = CheckAnfNodeIfCNodeAndInputSize(node, kBackendReshapeInputTensorNum);
  MS_EXCEPTION_IF_NULL(out_reshape);
  // If reshape operator used by more than one other operators, reshape operator cant not be deleted  directly
  if (IsUsedByOthers(func_graph, out_reshape)) {
    return nullptr;
  }
  auto in_reshape =
    CheckAnfNodeIfCNodeAndInputSize(AnfAlgo::GetInputNode(out_reshape, 0), kBackendReshapeInputTensorNum);
  MS_EXCEPTION_IF_NULL(in_reshape);
  if (IsUsedByOthers(func_graph, in_reshape)) {
    return nullptr;
  }

  if (AnfAlgo::IsDynamicShape(out_reshape) || AnfAlgo::IsDynamicShape(in_reshape)) {
    return nullptr;
  }
  auto output_shape = AnfAlgo::GetOutputDeviceShape(out_reshape, 0);
  auto input_shape = AnfAlgo::GetInputDeviceShape(in_reshape, 0);
  if (kernel::IsSameShape(input_shape, output_shape)) {
    auto input_node = AnfAlgo::GetInputNode(in_reshape, 0);
    return input_node;
  }
  return nullptr;
}
}  // namespace opt
}  // namespace luojianet_ms
