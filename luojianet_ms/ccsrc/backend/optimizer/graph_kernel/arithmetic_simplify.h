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
#ifndef LUOJIANET_MS_CCSRC_BACKEND_OPTIMIZER_GRAPH_KERNEL_ARITHMETIC_SIMPLIFY_H_
#define LUOJIANET_MS_CCSRC_BACKEND_OPTIMIZER_GRAPH_KERNEL_ARITHMETIC_SIMPLIFY_H_

#include <memory>
#include <vector>
#include <string>

#include "utils/hash_map.h"
#include "backend/optimizer/common/optimizer.h"
#include "ir/func_graph.h"
#include "backend/optimizer/graph_kernel/model/lite_graph.h"

namespace luojianet_ms::graphkernel {
class PatternTree;
using PatternTreePtr = std::shared_ptr<PatternTree>;
class ArithmeticSimplify : public opt::Pass {
 public:
  ArithmeticSimplify() : Pass("arithmetic_simplify") {}
  ~ArithmeticSimplify() override = default;
  bool Run(const FuncGraphPtr &func_graph) override;

 private:
  bool DoArithmeticTrans(const inner::LiteGraphPtr &litegraph);
  bool DoConstantFold(const inner::LiteGraphPtr &litegraph);
  luojianet_ms::HashMap<std::string, std::vector<PatternTreePtr>> expressions_map_;
};
using ArithmeticSimplifyPtr = std::shared_ptr<ArithmeticSimplify>;
}  // namespace luojianet_ms::graphkernel
#endif  // LUOJIANET_MS_CCSRC_BACKEND_OPTIMIZER_GRAPH_KERNEL_ARITHMETIC_SIMPLIFY_H_
