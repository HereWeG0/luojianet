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

#ifndef LUOJIANET_MS_CORE_OPS_SGD_H_
#define LUOJIANET_MS_CORE_OPS_SGD_H_
#include <vector>
#include <memory>
#include "ops/primitive_c.h"
#include "ops/op_utils.h"
#include "abstract/abstract_value.h"

namespace luojianet_ms {
namespace ops {
constexpr auto kNameSGD = "SGD";
/// \brief Computes the stochastic gradient descent.
/// Refer to Python API @ref luojianet_ms.ops.SGD for more details.
class MS_CORE_API SGD : public PrimitiveC {
 public:
  /// \brief Constructor.
  SGD() : PrimitiveC(kNameSGD) {}
  /// \brief Destructor.
  ~SGD() = default;
  MS_DECLARE_PARENT(SGD, PrimitiveC);
  /// \brief Init. Refer to the parameters of Python API @ref luojianet_ms.ops.SGD for the inputs.
  void Init(const float dampening = 0.0, const float weight_decay = 0.0, const bool nesterov = false);
  /// \brief Set dampening.
  void set_dampening(const float dampening);
  /// \brief Set weight_decay.
  void set_weight_decay(const float weight_decay);
  /// \brief Set nesterov.
  void set_nesterov(const bool nesterov);
  /// \brief Get dampening.
  ///
  /// \return dampening.
  float get_dampening() const;
  /// \brief Get weight_decay.
  ///
  /// \return weight_decay.
  float get_weight_decay() const;
  /// \brief Get nesterov.
  ///
  /// \return nesterov.
  bool get_nesterov() const;
};
AbstractBasePtr SGDInfer(const abstract::AnalysisEnginePtr &, const PrimitivePtr &primitive,
                         const std::vector<AbstractBasePtr> &input_args);
using PrimSGD = std::shared_ptr<SGD>;
}  // namespace ops
}  // namespace luojianet_ms
#endif  // LUOJIANET_MS_CORE_OPS_SGD_H_
