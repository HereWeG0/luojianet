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

#ifndef LUOJIANET_MS_LITE_EXAMPLES_RUNTIME_EXTEND_SRC_CUSTOM_COMMON_H
#define LUOJIANET_MS_LITE_EXAMPLES_RUNTIME_EXTEND_SRC_CUSTOM_COMMON_H

#include <vector>
#include "include/api/types.h"
#include "include/api/status.h"
#include "include/ms_tensor.h"

namespace luojianet_ms {
namespace common {
// verify that the inputs' shape is inferred successfully when inferring current node.
Status CheckInputs(const std::vector<luojianet_ms::MSTensor> &inputs);

// versify that the outputs' shape is inferred successfully when running current node.
Status CheckOutputs(const std::vector<luojianet_ms::MSTensor> &inputs);
}  // namespace common
}  // namespace luojianet_ms
#endif  // LUOJIANET_MS_LITE_EXAMPLES_RUNTIME_REGISTRY_SRC_CUSTOM_COMMON_H
