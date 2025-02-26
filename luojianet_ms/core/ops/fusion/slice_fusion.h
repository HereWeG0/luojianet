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

#ifndef LUOJIANET_MS_CORE_OPS_SLICE_FUSION_H_
#define LUOJIANET_MS_CORE_OPS_SLICE_FUSION_H_
#include <map>
#include <vector>
#include <string>
#include <memory>
#include "ops/primitive_c.h"
#include "abstract/abstract_value.h"
#include "utils/check_convert_utils.h"

namespace luojianet_ms {
namespace ops {
constexpr auto kNameSliceFusion = "SliceFusion";
/// \brief SliceFusion defined Slice operator prototype of lite.
class MS_CORE_API SliceFusion : public PrimitiveC {
 public:
  /// \brief Constructor.
  SliceFusion() : PrimitiveC(kNameSliceFusion) { InitIOName({"x", "begin", "size"}, {"output"}); }

  /// \brief Destructor.
  ~SliceFusion() = default;

  MS_DECLARE_PARENT(SliceFusion, PrimitiveC);

  /// \brief Method to init the op's attributes.
  ///
  /// \param[in] axes Define this operation will be performed on which axes.
  void Init(const std::vector<int64_t> &axes);

  /// \brief Method to set axes attribute.
  ///
  /// \param[in] axes Define this operation will be performed on which axes.
  void set_axes(const std::vector<int64_t> &axes);

  /// \brief Method to get axes attribute.
  ///
  /// \return axes.
  std::vector<int64_t> get_axes() const;
};
AbstractBasePtr SliceFusionInfer(const abstract::AnalysisEnginePtr &, const PrimitivePtr &primitive,
                                 const std::vector<AbstractBasePtr> &input_args);
}  // namespace ops
}  // namespace luojianet_ms

#endif  // LUOJIANET_MS_CORE_OPS_SLICE_FUSION_H_
