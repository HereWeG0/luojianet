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

#ifndef LUOJIANET_MS_CORE_OPS_CONSTANT_OF_SHAPE_H_
#define LUOJIANET_MS_CORE_OPS_CONSTANT_OF_SHAPE_H_
#include <memory>
#include <vector>
#include "ops/primitive_c.h"
#include "abstract/abstract_value.h"
#include "utils/check_convert_utils.h"

namespace luojianet_ms {
namespace ops {
constexpr auto kNameConstantOfShape = "ConstantOfShape";
/// \brief ConstantOfShape defined ConstantOfShape operator prototype of lite.
class MS_CORE_API ConstantOfShape : public PrimitiveC {
 public:
  /// \brief Constructor.
  ConstantOfShape() : PrimitiveC(kNameConstantOfShape) {}

  /// \brief Destructor.
  ~ConstantOfShape() = default;

  MS_DECLARE_PARENT(ConstantOfShape, PrimitiveC);

  /// \brief Method to init the op's attributes.
  ///
  /// \param[in] data_type Define data type of output data.
  /// \param[in] value Define value of output data.
  void Init(int64_t data_type, const std::vector<float> &value);

  /// \brief Method to set data type attribute.
  ///
  /// \param[in] data_type Define data type of output data.
  void set_data_type(int64_t data_type);

  /// \brief Method to set value attribute.
  ///
  /// \param[in] value Define value of output data.
  void set_value(const std::vector<float> &value);

  /// \brief Method to get data type attribute.
  ///
  /// \return data type attribute.
  int64_t get_data_type() const;

  /// \brief Method to get value attribute.
  ///
  /// \return value attribute.
  std::vector<float> get_value() const;
};

AbstractBasePtr ConstantOfShapeInfer(const abstract::AnalysisEnginePtr &, const PrimitivePtr &primitive,
                                     const std::vector<AbstractBasePtr> &input_args);
}  // namespace ops
}  // namespace luojianet_ms

#endif  // LUOJIANET_MS_CORE_OPS_CONSTANT_OF_SHAPE_H_
