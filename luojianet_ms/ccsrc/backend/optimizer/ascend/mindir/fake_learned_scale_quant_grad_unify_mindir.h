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
#ifndef LUOJIANET_MS_CCSRC_BACKEND_OPTIMIZER_ASCEND_MINDIR_FAKE_LEARNED_SCALE_QUANT_GRAD_UNIFY_MINDIR_H_
#define LUOJIANET_MS_CCSRC_BACKEND_OPTIMIZER_ASCEND_MINDIR_FAKE_LEARNED_SCALE_QUANT_GRAD_UNIFY_MINDIR_H_

#include <vector>
#include "backend/optimizer/common/optimizer.h"
#include "backend/optimizer/common/helper.h"

namespace luojianet_ms {
namespace opt {
constexpr size_t kFakeLearnedScaleQuantGradOutputNum = 2;
constexpr size_t kFakeLearnedScaleQuantGradInputNum = 5;
constexpr size_t kFakeLearnedScaleQuantGradDOutputNum = 2;
constexpr auto kFakeLearnedScaleQuantPerLayerGradOpName = "FakeLearnedScaleQuantPerLayerGrad";
constexpr auto kFakeLearnedScaleQuantPerLayerGradDOpName = "FakeLearnedScaleQuantPerLayerGradD";
constexpr auto kFakeLearnedScaleQuantPerLayerGradDReduceOpName = "FakeLearnedScaleQuantPerLayerGradDReduce";
constexpr auto kFakeLearnedScaleQuantPerChannelGradOpName = "FakeLearnedScaleQuantPerChannelGrad";
constexpr auto kFakeLearnedScaleQuantPerChannelGradDOpName = "FakeLearnedScaleQuantPerChannelGradD";
constexpr auto kFakeLearnedScaleQuantPerChannelGradDReduceOpName = "FakeLearnedScaleQuantPerChannelGradDReduce";

constexpr auto kAttrNeg_trunc = "neg_trunc";
constexpr auto kAttrChannelAxis = "channel_axis";

class FakeLearnedScaleQuantPerLayerGradUnifyMindIR : public PatternProcessPass {
 public:
  explicit FakeLearnedScaleQuantPerLayerGradUnifyMindIR(bool multigraph = true)
      : PatternProcessPass("fake_learned_scale_quant_perlayer_grad_unify_mindir", multigraph) {}
  ~FakeLearnedScaleQuantPerLayerGradUnifyMindIR() override = default;
  const BaseRef DefinePattern() const override;
  const AnfNodePtr Process(const FuncGraphPtr &, const AnfNodePtr &, const EquivPtr &) const override;

 private:
  void CreateOutputsOfLSQPerLayerGradD(const FuncGraphPtr &graph, const CNodePtr &lsq_perlayer_grad_node,
                                       std::vector<AnfNodePtr> *const lsq_perlayer_grad_d_outputs) const;
  void CreateOutputsOfLSQPerLayerReduceGrad(const FuncGraphPtr &graph, const CNodePtr &lsq_perlayer_grad_node,
                                            const std::vector<AnfNodePtr> &lsq_perlayer_grad_d_outputs,
                                            std::vector<AnfNodePtr> *const lsq_perlayer_reduce_grad_outputs) const;
};

class FakeLearnedScaleQuantPerChannelGradUnifyMindIR : public PatternProcessPass {
 public:
  explicit FakeLearnedScaleQuantPerChannelGradUnifyMindIR(bool multigraph = true)
      : PatternProcessPass("fake_learned_scale_quant_perchannel_grad_unify_mindir", multigraph) {}
  ~FakeLearnedScaleQuantPerChannelGradUnifyMindIR() override = default;
  const BaseRef DefinePattern() const override;
  const AnfNodePtr Process(const FuncGraphPtr &, const AnfNodePtr &, const EquivPtr &) const override;

 private:
  void CreateOutputsOfLSQPerChannelGradD(const FuncGraphPtr &graph, const CNodePtr &lsq_perchannel_grad_node,
                                         std::vector<AnfNodePtr> *const lsq_perchannel_grad_d_outputs) const;
  void CreateOutputsOfLSQPerChannelReduceGrad(const FuncGraphPtr &graph, const CNodePtr &lsq_perchannel_grad_node,
                                              const std::vector<AnfNodePtr> &lsq_perchannel_grad_d_outputs,
                                              std::vector<AnfNodePtr> *const lsq_perchannel_reduce_grad_outputs) const;
};

}  // namespace opt
}  // namespace luojianet_ms
#endif  // LUOJIANET_MS_CCSRC_BACKEND_OPTIMIZER_ASCEND_MINDIR_FAKE_LEARNED_SCALE_QUANT_GRAD_UNIFY_MINDIR_H_
