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

#ifndef LUOJIANET_MS_LITE_TOOLS_COMMON_STORAGE_H
#define LUOJIANET_MS_LITE_TOOLS_COMMON_STORAGE_H

#include <fstream>
#include <string>
#include "include/errorcode.h"
#include "flatbuffers/flatbuffers.h"
#include "schema/inner/model_generated.h"

namespace luojianet_ms {
namespace lite {
class Storage {
 public:
  static schema::MetaGraphT *Load(const std::string &inputPath);
};
}  // namespace lite
}  // namespace luojianet_ms

#endif  // LUOJIANET_MS_LITE_TOOLS_COMMON_STORAGE_H
