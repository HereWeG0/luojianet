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

#ifndef LUOJIANET_MS_CCSRC_FRONTEND_PARALLEL_TENSOR_LAYOUT_CONSTRUCT_OPERATOR_H_
#define LUOJIANET_MS_CCSRC_FRONTEND_PARALLEL_TENSOR_LAYOUT_CONSTRUCT_OPERATOR_H_

#include <string>
#include <utility>
#include <vector>

#include "ir/value.h"
#include "frontend/parallel/ops_info/operator_info.h"
#include "frontend/parallel/status.h"

namespace luojianet_ms {
namespace parallel {
using Args = std::vector<std::int64_t>;
Operator CreateStridedSliceOp(int64_t value, const Shape &begin, const Shape &end, const Shape &strides);
class ConstructOperator {
 public:
  const int64_t DEFAULT = 0;
  ConstructOperator() : dev_size_(0) {}
  ~ConstructOperator() = default;
  Status Init(const RankList &dev_list, const Shape &dev_matrix_shape);
  OperatorVector SkipRedisReshapeOP(const Shape &shape);
  Status ReshapeOP(const Shape &shape);
  Status StridedSliceOP(const Args &args);
  Status AllGatherOP(int64_t dev_dim);
  Status SplitOP(int64_t split_count);
  Status ConcatOP(int64_t concat_dim);
  Status AlltoAllOP(const Args &args);
  Operator GetOperator() const { return op_; }
  void UpdateTensorShape(const Shape &tensor_shape) { tensor_shape_ = tensor_shape; }

 private:
  Operator op_;
  size_t dev_size_;
  Shape tensor_shape_;
  RankList dev_list_;
  Shape dev_matrix_shape_;
  Status CreateGroupByDim(size_t axis, std::vector<Group> *group);
};
}  // namespace parallel
}  // namespace luojianet_ms

#endif  // LUOJIANET_MS_CCSRC_FRONTEND_PARALLEL_TENSOR_LAYOUT_CONSTRUCT_OPERATOR_H_
