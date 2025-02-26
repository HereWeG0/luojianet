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

#ifndef LUOJIANET_MS_CORE_OPS_REVERSE_SEQUENCE_H_
#define LUOJIANET_MS_CORE_OPS_REVERSE_SEQUENCE_H_
#include <vector>
#include <memory>
#include "ops/primitive_c.h"
#include "abstract/abstract_value.h"
#include "utils/check_convert_utils.h"

namespace luojianet_ms {
namespace ops {
constexpr auto kNameReverseSequence = "ReverseSequence";
/// \brief Reverses variable length slices.
/// Refer to Python API @ref luojianet_ms.ops.ReverseSequence for more details.
class MS_CORE_API ReverseSequence : public PrimitiveC {
 public:
  /// \brief Constructor.
  ReverseSequence() : PrimitiveC(kNameReverseSequence) { InitIOName({"x", "seq_lengths"}, {"y"}); }
  /// \brief Destructor.
  ~ReverseSequence() = default;
  MS_DECLARE_PARENT(ReverseSequence, PrimitiveC);
  /// \brief Init. Refer to the parameters of Python API @ref luojianet_ms.ops.ReverseSequence for the inputs.
  void Init(const int64_t seq_dim, const int64_t batch_dim = 0);
  /// \brief Set seq_dim.
  void set_seq_dim(const int64_t seq_dim);
  /// \brief Set batch_dim.
  void set_batch_dim(const int64_t batch_dim);
  /// \brief Get seq_dim.
  ///
  /// \return seq_dim.
  int64_t get_seq_dim() const;
  /// \brief Get batch_dim.
  ///
  /// \return batch_dim.
  int64_t get_batch_dim() const;
};
AbstractBasePtr ReverseSequenceInfer(const abstract::AnalysisEnginePtr &, const PrimitivePtr &primitive,
                                     const std::vector<AbstractBasePtr> &input_args);
using PrimReverseSequence = std::shared_ptr<ReverseSequence>;
}  // namespace ops
}  // namespace luojianet_ms

#endif  // LUOJIANET_MS_CORE_OPS_REVERSE_SEQUENCE_H_
