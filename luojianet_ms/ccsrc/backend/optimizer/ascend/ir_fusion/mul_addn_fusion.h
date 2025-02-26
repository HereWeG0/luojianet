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
#ifndef LUOJIANET_MS_CCSRC_BACKEND_OPTIMIZER_ASCEND_IR_FUSION_PASS_MUL_ADDN_FUSION_H
#define LUOJIANET_MS_CCSRC_BACKEND_OPTIMIZER_ASCEND_IR_FUSION_PASS_MUL_ADDN_FUSION_H

#include "backend/optimizer/ascend/ascend_pass_control.h"

namespace luojianet_ms {
namespace opt {
class MulAddNFusion : public PatternProcessPassWithSwitch {
 public:
  explicit MulAddNFusion(bool multigraph = true) : PatternProcessPassWithSwitch("mul_addn_fusion", multigraph) {
    PassSwitchManager::GetInstance().RegistLicPass(name(), OptPassEnum::MulAddNPass);
  }
  ~MulAddNFusion() override = default;
  const BaseRef DefinePattern() const override;
  const AnfNodePtr Process(const FuncGraphPtr &, const AnfNodePtr &, const EquivPtr &) const override;

 private:
  CNodePtr CreateFusionNode(const FuncGraphPtr &graph, const CNodePtr &mul, const CNodePtr &addn,
                            const size_t &lossscale_input_index) const;
};
}  // namespace opt
}  // namespace luojianet_ms
#endif  // LUOJIANET_MS_CCSRC_BACKEND_OPTIMIZER_ASCEND_IR_FUSION_PASS_MUL_ADDN_FUSION_H
