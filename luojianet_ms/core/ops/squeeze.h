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

#ifndef LUOJIANET_MS_CORE_OPS_SQUEEZE_H_
#define LUOJIANET_MS_CORE_OPS_SQUEEZE_H_

#include <map>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include "ops/op_utils.h"
#include "ops/primitive_c.h"
#include "abstract/primitive_infer_map.h"
#include "abstract/abstract_value.h"
#include "utils/check_convert_utils.h"

namespace luojianet_ms {
namespace ops {
constexpr auto kNameSqueeze = "Squeeze";
/// \brief Returns a tensor with the same data type but dimensions of 1 are removed based on axis.
/// Refer to Python API @ref luojianet_ms.ops.Squeeze for more details.
class MS_CORE_API Squeeze : public PrimitiveC {
 public:
  /// \brief Constructor.
  Squeeze() : PrimitiveC(kNameSqueeze) { InitIOName({"x"}, {"output"}); }
  /// \brief Destructor.
  ~Squeeze() = default;
  MS_DECLARE_PARENT(Squeeze, PrimitiveC);
  /// \brief Init. Refer to the parameters of Python API @ref luojianet_ms.ops.Squeeze for the inputs.
  void Init(const std::vector<int64_t> &axis = {});
  /// \brief Set axis.
  void set_axis(const std::vector<int64_t> &axis);
  /// \brief Get axis.
  ///
  /// \return axis.
  std::vector<int64_t> get_axis() const;
};

AbstractBasePtr SqueezeInfer(const abstract::AnalysisEnginePtr &, const PrimitivePtr &primitive,
                             const std::vector<AbstractBasePtr> &input_args);
}  // namespace ops
}  // namespace luojianet_ms

#endif  // LUOJIANET_MS_CORE_OPS_SQUEEZE_H_
