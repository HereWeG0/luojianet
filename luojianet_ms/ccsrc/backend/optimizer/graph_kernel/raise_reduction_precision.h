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
#ifndef LUOJIANET_MS_CCSRC_BACKEND_OPTIMIZER_RAISE_REDUCTION_PRECISION_H_
#define LUOJIANET_MS_CCSRC_BACKEND_OPTIMIZER_RAISE_REDUCTION_PRECISION_H_

#include <string>
#include "backend/optimizer/common/optimizer.h"

namespace luojianet_ms::graphkernel {
class RaiseReductionPrecision : public opt::Pass {
 public:
  RaiseReductionPrecision() : Pass("raise_reduction_precision") {}
  ~RaiseReductionPrecision() override = default;
  bool Run(const FuncGraphPtr &func_graph) override;

 private:
  bool IsFp16ReduceSum(const AnfNodePtr &node) const;
  bool Process(const FuncGraphPtr &func_graph);
  AnfNodePtr CreateCast(const AnfNodePtr &input, const TypePtr &dst_type, const std::string &format) const;
  AnfNodePtr CreateReduceSum(const AnfNodePtr &node, const AnfNodePtr &input) const;
  void ReplaceNode(const AnfNodePtr &src_node, const AnfNodePtr &dst_node) const;
};
}  // namespace luojianet_ms::graphkernel
#endif  // LUOJIANET_MS_CCSRC_BACKEND_OPTIMIZER_RAISE_REDUCTION_PRECISION_H_
