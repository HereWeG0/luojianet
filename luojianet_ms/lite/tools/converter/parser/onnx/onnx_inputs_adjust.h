/**
 * Copyright 2020-2021 Huawei Technologies Co., Ltd
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

#ifndef LUOJIANET_MS_LITE_TOOLS_CONVERTER_PARSER_ONNX_ONNX_INPUTS_ADJUST_H_
#define LUOJIANET_MS_LITE_TOOLS_CONVERTER_PARSER_ONNX_ONNX_INPUTS_ADJUST_H_
#include <string>
#include <vector>
#include "backend/optimizer/common/pass.h"
#include "tools/converter/converter_flags.h"
#include "tools/optimizer/common/gllo_utils.h"

using luojianet_ms::converter::FmkType;
namespace luojianet_ms::lite {
class OnnxInputAdjust {
 public:
  static bool Adjust(const FuncGraphPtr &func_graph);
};
}  // namespace luojianet_ms::lite
#endif  // LUOJIANET_MS_LITE_TOOLS_CONVERTER_PARSER_ONNX_ONNX_INPUTS_ADJUST_H_
