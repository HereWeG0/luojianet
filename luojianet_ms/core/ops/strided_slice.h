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

#ifndef LUOJIANET_MS_CORE_OPS_STRIDED_SLICE_H_
#define LUOJIANET_MS_CORE_OPS_STRIDED_SLICE_H_
#include <map>
#include <vector>
#include <string>
#include <memory>
#include "ops/primitive_c.h"
#include "abstract/abstract_value.h"
#include "utils/check_convert_utils.h"

namespace luojianet_ms {
namespace ops {
constexpr auto kNameStridedSlice = prim::kStridedSlice;
/// \brief Extracts a strided slice of a tensor. Refer to Python API @ref luojianet_ms.ops.StridedSlice for more details.
class MS_CORE_API StridedSlice : public PrimitiveC {
 public:
  /// \brief Constructor.
  StridedSlice() : PrimitiveC(prim::kPrimStridedSlice->name()) {
    InitIOName({"x", "begin", "end", "strides"}, {"output"});
  }
  /// \brief Destructor.
  ~StridedSlice() = default;
  MS_DECLARE_PARENT(StridedSlice, PrimitiveC);
  /// \brief Init. Refer to the parameters of python API @ref luojianet_ms.ops.StridedSlice for the inputs.
  void Init(int64_t begin_mask = 0, int64_t end_mask = 0, int64_t ellipsis_mask = 0, int64_t new_axis_mask = 0,
            int64_t shrink_axis_mask = 0);
  /// \brief Set begin_mask.
  void set_begin_mask(int64_t begin_mask);
  /// \brief Set end_mask.
  void set_end_mask(int64_t end_mask);
  /// \brief Set ellipsis_mask.
  void set_ellipsis_mask(int64_t ellipsis_mask);
  /// \brief Set new_axis_mask.
  void set_new_axis_mask(int64_t new_axis_mask);
  /// \brief Set shrink_axis_mask.
  void set_shrink_axis_mask(int64_t shrink_axis_mask);
  /// \brief Get begin_mask.
  ///
  /// \return begin_mask.
  int64_t get_begin_mask() const;
  /// \brief Get end_mask.
  ///
  /// \return end_mask.
  int64_t get_end_mask() const;
  /// \brief Get ellipsis_mask.
  ///
  /// \return ellipsis_mask.
  int64_t get_ellipsis_mask() const;
  /// \brief Get new_axis_mask.
  ///
  /// \return new_axis_mask.
  int64_t get_new_axis_mask() const;
  /// \brief Get shrink_axis_mask.
  ///
  /// \return shrink_axis_mask.
  int64_t get_shrink_axis_mask() const;
};

AbstractBasePtr StridedSliceInfer(const abstract::AnalysisEnginePtr &, const PrimitivePtr &primitive,
                                  const std::vector<AbstractBasePtr> &input_args);
using PrimStridedSlicePtr = std::shared_ptr<StridedSlice>;
}  // namespace ops
}  // namespace luojianet_ms

#endif  // LUOJIANET_MS_CORE_OPS_STRIDED_SLICE_H_
