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

#ifndef LUOJIANET_MS_CORE_OPS_BATCH_NORMAL_H_
#define LUOJIANET_MS_CORE_OPS_BATCH_NORMAL_H_
#include <map>
#include <vector>
#include <memory>
#include <string>
#include "ops/op_utils.h"
#include "ops/primitive_c.h"
#include "abstract/abstract_value.h"

namespace luojianet_ms {
namespace ops {
constexpr auto kNameBatchNorm = "BatchNorm";
/// \brief Batch Normalization for input data and updated parameters.
/// Refer to Python API @ref luojianet_ms.ops.BatchNorm for more details.
class MS_CORE_API BatchNorm : public PrimitiveC {
 public:
  /// \brief Constructor.
  BatchNorm() : PrimitiveC(kNameBatchNorm) {
    InitIOName({"x", "scale", "offset", "mean", "variance"},
               {"y", "batch_mean", "batch_variance", "reserve_space_1", "reserve_space_2"});
  }
  /// \brief Destructor.
  ~BatchNorm() = default;
  MS_DECLARE_PARENT(BatchNorm, PrimitiveC);
  /// \brief Init. Refer to the parameters of Python API @ref luojianet_ms.ops.BatchNorm for the inputs.
  void Init(const bool is_training = false, const float epsilon = 1e-5, const float momentun = 0.1,
            const Format &format = NCHW);
  /// \brief Set is_training.
  void set_is_training(const bool is_training);
  /// \brief Set epsilon.
  void set_epsilon(const float epsilon);
  /// \brief Set format.
  void set_format(const Format &format);
  /// \brief Set momentum.
  void set_momentum(const float momentum);
  /// \brief Get is_training.
  ///
  /// \return is_training.
  bool get_is_training() const;
  /// \brief Get epsilon.
  ///
  /// \return epsilon.
  float get_epsilon() const;
  /// \brief Get format.
  ///
  /// \return format.
  Format get_format() const;
  /// \brief Get momentum.
  ///
  /// \return momentum.
  float get_momentum() const;
};

AbstractBasePtr BatchNormInfer(const abstract::AnalysisEnginePtr &, const PrimitivePtr &primitive,
                               const std::vector<AbstractBasePtr> &input_args);
using PrimBatchNormPtr = std::shared_ptr<BatchNorm>;

}  // namespace ops
}  // namespace luojianet_ms

#endif  // LUOJIANET_MS_CORE_OPS_BatchNorm_H_
