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

#ifndef LUOJIANET_MS_CCSRC_BACKEND_OPTIMIZER_GRAPH_KERNEL_REORDER_OPS_H_
#define LUOJIANET_MS_CCSRC_BACKEND_OPTIMIZER_GRAPH_KERNEL_REORDER_OPS_H_

#include <memory>
#include <vector>
#include <string>
#include "backend/optimizer/common/pass.h"

namespace luojianet_ms::graphkernel {
struct NodeIOInfo {
  std::vector<std::string> inputs_format;
  std::vector<std::string> outputs_format;
  std::vector<TypeId> inputs_type;
  std::vector<TypeId> outputs_type;
};

class ReorderOps : public opt::Pass {
 public:
  ReorderOps() : Pass("reorder_ops") {}
  ~ReorderOps() override = default;
  bool Run(const FuncGraphPtr &func_graph) override;

 private:
  void SetTypeInsensitiveNodeInputs(const CNodePtr &node, const std::vector<size_t> &indexes,
                                    const std::vector<AnfNodePtr> &new_input_in_indexes,
                                    std::vector<AnfNodePtr> *new_inputs);
  void SetTypeInsensitiveNodeInputsInfo(const CNodePtr &node, const std::vector<size_t> &indexes,
                                        const std::vector<AnfNodePtr> &input_at_indexes, NodeIOInfo *new_inputs_info,
                                        bool from_input);
  bool ReorderTypeInsensitiveCastDown(const FuncGraphPtr &func_graph, const FuncGraphManagerPtr &mng,
                                      const CNodePtr &node);
  bool ReorderCastUpTypeInsensitive(const FuncGraphPtr &func_graph, const FuncGraphManagerPtr &mng,
                                    const CNodePtr &node);
  bool ReorderCastTypeInsensitive(const FuncGraphPtr &func_graph);
};
using ReorderOpsPtr = std::shared_ptr<ReorderOps>;
}  // namespace luojianet_ms::graphkernel
#endif  // LUOJIANET_MS_CCSRC_BACKEND_OPTIMIZER_GRAPH_KERNEL_REORDER_OPS_H_
