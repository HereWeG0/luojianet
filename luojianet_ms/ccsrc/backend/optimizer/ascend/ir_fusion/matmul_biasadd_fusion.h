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
#ifndef LUOJIANET_MS_CCSRC_BACKEND_OPTIMIZER_ASCEND_IR_FUSION_MATMUL_BIASADD_FUSION_H_
#define LUOJIANET_MS_CCSRC_BACKEND_OPTIMIZER_ASCEND_IR_FUSION_MATMUL_BIASADD_FUSION_H_

#include <memory>
#include <string>
#include "backend/optimizer/ascend/ascend_pass_control.h"

namespace luojianet_ms {
namespace opt {
class MatmulBiasaddFusion : public PatternProcessPassWithSwitch {
 public:
  explicit MatmulBiasaddFusion(bool multigraph = true, string pass_name = "matmul_biasadd_fusion")
      : PatternProcessPassWithSwitch(pass_name, multigraph) {
    x0_ = std::make_shared<Var>();
    x1_ = std::make_shared<Var>();
    x2_ = std::make_shared<Var>();
    matmul_var_ = std::make_shared<Var>(std::make_shared<Primitive>(prim::kPrimMatMul->name()));
    PassSwitchManager::GetInstance().RegistLicPass(name(), OptPassEnum::MatmulBiasaddFusion);
  }
  ~MatmulBiasaddFusion() override = default;
  const BaseRef DefinePattern() const override;
  const AnfNodePtr Process(const FuncGraphPtr &, const AnfNodePtr &, const EquivPtr &) const override;

 protected:
  AnfNodePtr CreateMatmulWithBias(const FuncGraphPtr &graph, const AnfNodePtr &node, const EquivPtr &equiv) const;
  VarPtr x0_;
  VarPtr x1_;
  VarPtr x2_;
  VarPtr matmul_var_;
};

class MatmulAddFusion : public MatmulBiasaddFusion {
 public:
  explicit MatmulAddFusion(bool multigraph = true) : MatmulBiasaddFusion(multigraph, "matmul_add_fusion") {}
  ~MatmulAddFusion() override = default;
  const BaseRef DefinePattern() const override;
  const AnfNodePtr Process(const FuncGraphPtr &, const AnfNodePtr &, const EquivPtr &) const override;

 private:
  bool NeedFusion(const AnfNodePtr &add) const;
};
}  // namespace opt
}  // namespace luojianet_ms

#endif  // LUOJIANET_MS_CCSRC_BACKEND_OPTIMIZER_ASCEND_IR_FUSION_MATMUL_BIASADD_FUSION_H_
