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

#ifndef LUOJIANET_MS_CCSRC_BACKEND_OPTIMIZER_ASCEND_IR_FUSION_TRANSPOSE_TRANSDATA_FUSION_H_
#define LUOJIANET_MS_CCSRC_BACKEND_OPTIMIZER_ASCEND_IR_FUSION_TRANSPOSE_TRANSDATA_FUSION_H_

#include <vector>
#include <string>
#include <utility>
#include <memory>
#include "ir/anf.h"
#include "backend/optimizer/common/pattern_engine.h"
#include "backend/optimizer/common/helper.h"
#include "backend/optimizer/common/optimizer.h"
#include "backend/optimizer/ascend/ascend_helper.h"

namespace luojianet_ms {
namespace opt {
class TransposeTransDataFusion : public PatternProcessPass {
 public:
  explicit TransposeTransDataFusion(bool multigraph = true)
      : PatternProcessPass("transpose_transdata_fusion", multigraph) {
    input_varptr_ = std::make_shared<Var>();
    supported_checker_ = std::make_shared<SupportedChecker>();
  }
  ~TransposeTransDataFusion() override = default;
  const BaseRef DefinePattern() const override;
  const AnfNodePtr Process(const FuncGraphPtr &, const AnfNodePtr &, const EquivPtr &) const override;

 private:
  VarPtr input_varptr_;

 private:
  SupportedCheckerPtr supported_checker_;
};
}  // namespace opt
}  // namespace luojianet_ms

#endif  // LUOJIANET_MS_CCSRC_BACKEND_OPTIMIZER_ASCEND_IR_FUSION_TRANSPOSE_TRANSDATA_FUSION_H_
