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

#ifndef LUOJIANET_MS_LITE_MICRO_CODER_GENERATOR_TRAIN_COMPONENT_H_
#define LUOJIANET_MS_LITE_MICRO_CODER_GENERATOR_TRAIN_COMPONENT_H_

#include <map>
#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include "src/tensor.h"
#include "coder/context.h"

namespace luojianet_ms::lite::micro {
void CodeTrainParams(std::ofstream &ofs);

void CodeFeaturesState(std::ofstream &ofs);
void CodeFeaturesImplement(std::ofstream &ofs, const std::unique_ptr<CoderContext> &ctx);

void CodeTrainState(std::ofstream &ofs);
void CodeTrainImplement(std::ofstream &ofs, const std::unique_ptr<CoderContext> &ctx);
}  // namespace luojianet_ms::lite::micro

#endif  // LUOJIANET_MS_LITE_MICRO_CODER_GENERATOR_TRAIN_COMPONENT_H_
