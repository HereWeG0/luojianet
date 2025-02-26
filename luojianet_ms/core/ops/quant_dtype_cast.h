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

#ifndef LUOJIANET_MS_CORE_OPS_QUANTD_TYPE_CAST_H_
#define LUOJIANET_MS_CORE_OPS_QUANTD_TYPE_CAST_H_

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
constexpr auto kNameQuantDTypeCast = "QuantDTypeCast";
/// \brief QuantDTypeCast QuantDTypeCast the QuantDTypeCast operator prototype.
class MS_CORE_API QuantDTypeCast : public PrimitiveC {
 public:
  /// \brief Constructor.
  QuantDTypeCast() : PrimitiveC(kNameQuantDTypeCast) {}

  /// \brief Destructor.
  ~QuantDTypeCast() = default;

  MS_DECLARE_PARENT(QuantDTypeCast, PrimitiveC);

  /// \brief Method to init the op's attributes.
  ///
  /// \param[in] src_t Define the data type of input.
  /// \param[in] dst_t Define the data type of output.
  void Init(const int64_t src_t, const int64_t dst_t);

  /// \brief Method to set src_t attribute.
  ///
  /// \param[in] src_t Define the data type of input.
  void set_src_t(const int64_t src_t);

  /// \brief Method to get src_t attribute.
  ///
  /// \return the data type of input.
  int64_t get_src_t() const;

  /// \brief Method to set dst_t attribute.
  ///
  /// \param[in] dst_t Define the data type of output.
  void set_dst_t(const int64_t dst_t);

  /// \brief Method to get dst_t attribute.
  ///
  /// \return the data type of output.
  int64_t get_dst_t() const;
};
AbstractBasePtr QuantDTypeCastInfer(const abstract::AnalysisEnginePtr &, const PrimitivePtr &primitive,
                                    const std::vector<AbstractBasePtr> &input_args);
}  // namespace ops
}  // namespace luojianet_ms

#endif  // LUOJIANET_MS_CORE_OPS_QUANTD_TYPE_CAST_H_
