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

#ifndef INC_EXTERNAL_GRAPH_ATTR_VALUE_H_
#define INC_EXTERNAL_GRAPH_ATTR_VALUE_H_

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "./ge_error_codes.h"
#include "ascend_string.h"

using std::to_string;
using std::unique_ptr;
namespace ge {
class AttrValueImpl;
/*lint -e148*/
class GE_FUNC_DEV_VISIBILITY GE_FUNC_HOST_VISIBILITY AttrValue {
 public:
  using INT = int64_t;
  using FLOAT = float;
  using STR = std::string;

  AttrValue();
  ~AttrValue() = default;

  // GetValue, not list type
  template <typename T, typename DT>
  graphStatus GetValue(DT &val) const {
    T valGet;
    const auto status = GetValue(valGet);
    if (status != GRAPH_SUCCESS) {
      return status;
    }
    val = DT(valGet);
    return GRAPH_SUCCESS;
  }

  template <typename T, typename DT>
  static T CreateFrom(DT &&val) {
    return val;
  }

  graphStatus GetValue(AscendString &val);

  std::shared_ptr<AttrValueImpl> impl;

 private:
#define VALUE_SET_GET_DEC(DT) graphStatus GetValue(DT &val) const;
  VALUE_SET_GET_DEC(AttrValue::STR)
  VALUE_SET_GET_DEC(AttrValue::INT)
  VALUE_SET_GET_DEC(AttrValue::FLOAT)
#undef VALUE_SET_GET_DEC
};
/*lint +e148*/
}  // namespace ge
#endif  // INC_EXTERNAL_GRAPH_ATTR_VALUE_H_
