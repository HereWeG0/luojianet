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

#ifndef LUOJIANET_MS_CORE_OPS_ONE_HOT_H_
#define LUOJIANET_MS_CORE_OPS_ONE_HOT_H_
#include <vector>
#include <memory>

#include "ops/primitive_c.h"
#include "abstract/abstract_value.h"
#include "utils/check_convert_utils.h"

namespace luojianet_ms {
namespace ops {
/// \brief Computes a one-hot tensor. Refer to Python API @ref luojianet_ms.ops.OneHot for more details.
class MS_CORE_API OneHot : public PrimitiveC {
 public:
  /// \brief Constructor.
  OneHot() : PrimitiveC(prim::kPrimOneHot->name()) {
    InitIOName({"indices", "depth", "on_value", "off_value"}, {"output"});
  }
  /// \brief Destructor.
  ~OneHot() = default;
  MS_DECLARE_PARENT(OneHot, PrimitiveC);
  /// \brief Init. Refer to the parameters of Python API @ref luojianet_ms.ops.OneHot for the inputs.
  void Init(const int64_t axis);
  /// \brief Set axis.
  void set_axis(const int64_t axis);
  /// \brief Get axis.
  ///
  /// \return axis.
  int64_t get_axis() const;
};
}  // namespace ops
}  // namespace luojianet_ms

#endif  // LUOJIANET_MS_CORE_OPS_ONE_HOT_H_
