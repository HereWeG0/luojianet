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

#ifndef LUOJIANET_MS_CCSRC_BACKEND_OPTIMIZER_ASCEND_FORMAT_TYPE_DYNAMIC_RNN_GRAD_REFORMAT_H_
#define LUOJIANET_MS_CCSRC_BACKEND_OPTIMIZER_ASCEND_FORMAT_TYPE_DYNAMIC_RNN_GRAD_REFORMAT_H_
#include <utility>
#include "ir/anf.h"
#include "backend/optimizer/common/pattern_engine.h"
#include "backend/optimizer/common/helper.h"
#include "backend/optimizer/common/optimizer.h"

namespace luojianet_ms {
namespace opt {
class DynamicRNNGradReformat : public PatternProcessPass {
 public:
  explicit DynamicRNNGradReformat(bool multigraph = true)
      : PatternProcessPass("dynamic_rnn_grad_reformat", multigraph) {}
  ~DynamicRNNGradReformat() override = default;
  const BaseRef DefinePattern() const override;
  const AnfNodePtr Process(const FuncGraphPtr &, const AnfNodePtr &, const EquivPtr &) const override;
};
}  // namespace opt
}  // namespace luojianet_ms

#endif  // LUOJIANET_MS_CCSRC_BACKEND_OPTIMIZER_ASCEND_FORMAT_TYPE_DYNAMIC_RNN_GRAD_REFORMAT_H_
