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

#ifndef LUOJIANET_MS_CORE_OPS_TENSOR_LIST_GET_ITEM_H_
#define LUOJIANET_MS_CORE_OPS_TENSOR_LIST_GET_ITEM_H_
#include <memory>
#include "ops/primitive_c.h"
#include "abstract/abstract_value.h"
#include "utils/check_convert_utils.h"

namespace luojianet_ms {
namespace ops {
constexpr auto kNameTensorListGetItem = "TensorListGetItem";
/// \brief TensorListGetItem defined TensorListGetItem operator prototype of lite.
class MS_CORE_API TensorListGetItem : public PrimitiveC {
 public:
  /// \brief Constructor.
  TensorListGetItem() : PrimitiveC(kNameTensorListGetItem) {}
  /// \brief Destructor.
  ~TensorListGetItem() = default;

  MS_DECLARE_PARENT(TensorListGetItem, PrimitiveC);

  /// \brief Method to init the op's attributes.
  ///
  /// \param[in] element_dtype Define the element data type in op;
  void Init(const int64_t element_dtype);

  /// \brief Method to set the op's element_dtype attributes.
  ///
  /// \param[in] element_dtype Define the element data type in op;
  void set_element_dtype(const int64_t element_dtype);

  /// \brief Method to get the op's element_dtype attributes.
  int64_t get_element_dtype() const;
};
}  // namespace ops
}  // namespace luojianet_ms

#endif  // LUOJIANET_MS_CORE_OPS_TENSOR_LIST_GET_ITEM_H_
