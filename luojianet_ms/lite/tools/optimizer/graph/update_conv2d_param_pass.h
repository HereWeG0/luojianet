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

#ifndef LUOJIANET_MS_LITE_SRC_PASS_UPDATE_CONV2D_PARAM_PASS_H_
#define LUOJIANET_MS_LITE_SRC_PASS_UPDATE_CONV2D_PARAM_PASS_H_

#include "backend/optimizer/common/pass.h"
#include "tools/optimizer/common/gllo_utils.h"

namespace luojianet_ms::opt {
class UpdateConv2DParamPass : public Pass {
 public:
  UpdateConv2DParamPass() : Pass("UpdateConv2DParamPass") {}
  ~UpdateConv2DParamPass() override = default;
  bool Run(const FuncGraphPtr &graph) override;

 private:
  STATUS UpdateConv2DAttr(const CNodePtr &cnode);
};
}  // namespace luojianet_ms::opt
#endif  // LUOJIANET_MS_LITE_SRC_PASS_UPDATE_CONV2D_PARAM_PASS_H_
