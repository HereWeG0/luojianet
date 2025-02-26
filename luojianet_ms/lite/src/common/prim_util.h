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

#ifndef LUOJIANET_MS_LITE_SRC_COMMON_PRIM_UTIL_H_
#define LUOJIANET_MS_LITE_SRC_COMMON_PRIM_UTIL_H_

namespace luojianet_ms {
namespace lite {
int GetPrimitiveType(const void *prim, int schema_version);
const char *GetPrimitiveTypeName(const void *primitive, int schema_version);
const char *PrimitiveCurVersionTypeName(int type);
int GenPrimVersionKey(int primitive_type, int schema_version);
bool IsPartialNode(const void *primitive, int schema_version);
bool IsCallNode(const void *node, int schema_version);
bool IsSwitchNode(const void *node, int schema_version);
bool IsSwitchLayerNode(const void *node, int schema_version);
bool IsCustomNode(const void *primitive, int schema_version);
bool IsCastNode(const void *primitive, int schema_version);
int GetPartialGraphIndex(const void *primitive, int schema_version);
}  // namespace lite
}  // namespace luojianet_ms

#endif  // LUOJIANET_MS_LITE_SRC_COMMON_PRIM_UTIL_H_
