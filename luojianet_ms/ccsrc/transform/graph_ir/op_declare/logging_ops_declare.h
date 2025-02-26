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

#ifndef LUOJIANET_MS_CCSRC_TRANSFORM_GRAPH_IR_OP_DECLARE_LOGGING_OPS_DECLARE_H_
#define LUOJIANET_MS_CCSRC_TRANSFORM_GRAPH_IR_OP_DECLARE_LOGGING_OPS_DECLARE_H_

#include <string>
#include "utils/hash_map.h"
#include "transform/graph_ir/op_declare/op_declare_macro.h"
#include "ops/logging_ops.h"

namespace luojianet_ms::transform {
DECLARE_OP_ADAPTER(Print)
DECLARE_OP_USE_DYN_INPUT(Print)

#ifdef ENABLE_GE
DECLARE_OP_ADAPTER(Assert)
DECLARE_OP_USE_DYN_INPUT(Assert)
#endif
}  // namespace luojianet_ms::transform
#endif  // LUOJIANET_MS_CCSRC_TRANSFORM_GRAPH_IR_OP_DECLARE_LOGGING_OPS_DECLARE_H_
