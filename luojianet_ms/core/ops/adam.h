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

#ifndef LUOJIANET_MS_CORE_OPS_ADAM_H_
#define LUOJIANET_MS_CORE_OPS_ADAM_H_
#include <map>
#include <vector>
#include <string>
#include <memory>
#include "ops/primitive_c.h"
#include "abstract/abstract_value.h"
#include "utils/check_convert_utils.h"

namespace luojianet_ms {
namespace ops {
constexpr auto kNameAdam = "Adam";
/// \brief Updates gradients by the Adaptive Moment Estimation (Adam) algorithm.
/// Refer to Python API @ref luojianet_ms.ops.Adam for more details.
class MS_CORE_API Adam : public PrimitiveC {
 public:
  /// \brief Constructor.
  Adam() : PrimitiveC(kNameAdam) {}
  /// \brief Destructor.
  ~Adam() = default;
  MS_DECLARE_PARENT(Adam, PrimitiveC);
  /// \brief Init. Refer to the parameters of Python API @ref luojianet_ms.ops.Adam for the inputs.
  void Init(const bool use_locking = false, const bool use_nesterov = false);
  /// \brief Set use_locking.
  void set_use_locking(const bool use_locking);
  /// \brief Set use_nesterov.
  void set_use_nesterov(const bool use_nesterov);
  /// \brief Get use_locking.
  ///
  /// \return use_locking.
  bool get_use_locking() const;
  /// \brief Get use_nesterov.
  ///
  /// \return use_nesterov.
  bool get_use_nesterov() const;
};
AbstractBasePtr AdamInfer(const abstract::AnalysisEnginePtr &, const PrimitivePtr &primitive,
                          const std::vector<AbstractBasePtr> &input_args);
using kPrimAdamPtr = std::shared_ptr<Adam>;
}  // namespace ops
}  // namespace luojianet_ms

#endif  // LUOJIANET_MS_CORE_OPS_ADAM_H_
