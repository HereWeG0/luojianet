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
#ifndef LUOJIANET_MS_CCSRC_BACKEND_OPTIMIZER_GRAPH_KERNEL_GRAPH_KERNEL_SPLITTER_H_
#define LUOJIANET_MS_CCSRC_BACKEND_OPTIMIZER_GRAPH_KERNEL_GRAPH_KERNEL_SPLITTER_H_
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include "ir/func_graph.h"
#include "backend/optimizer/common/pass.h"

namespace luojianet_ms::graphkernel {
struct StitchInfo {
  std::vector<std::string> stitch_ops;
  std::vector<std::string> stitch_atomic_ops;
};

class SplitNodesDecoder {
 public:
  SplitNodesDecoder() {}
  ~SplitNodesDecoder() = default;
  static bool DecodeSplitNodes(const nlohmann::json &kernel_json,
                               const std::map<std::string, AnfNodePtr> &address_node_map, AnfNodePtrList *res_graphs);
};

class GraphKernelSplitter : public opt::Pass {
 public:
  GraphKernelSplitter() : Pass("graph_kernel_splitter") {}
  ~GraphKernelSplitter() override = default;
  bool Run(const FuncGraphPtr &func_graph) override;
};
using GraphKernelSplitterPtr = std::shared_ptr<GraphKernelSplitter>;
}  // namespace luojianet_ms::graphkernel
#endif  // LUOJIANET_MS_CCSRC_BACKEND_OPTIMIZER_GRAPH_KERNEL_GRAPH_KERNEL_SPLITTER_H_
