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

#ifndef LUOJIANET_MS_CORE_OPS_TANH_GRAD_H_
#define LUOJIANET_MS_CORE_OPS_TANH_GRAD_H_
#include <map>
#include <vector>
#include <string>
#include <memory>

#include "ops/primitive_c.h"
#include "abstract/abstract_value.h"
#include "utils/check_convert_utils.h"

namespace luojianet_ms {
namespace ops {
constexpr auto kNameTanhGrad = "TanhGrad";
class TanhGrad : public PrimitiveC {
 public:
  /// \brief Constructor.
  TanhGrad() : PrimitiveC(kNameTanhGrad) { InitIOName({"x"}, {"output"}); }
  /// \brief Destructor.
  ~TanhGrad() = default;
  /// \brief Init.
  MS_DECLARE_PARENT(TanhGrad, PrimitiveC);
};
}  // namespace ops
}  // namespace luojianet_ms

#endif  // LUOJIANET_MS_CORE_OPS_TanhGRAD_H_
