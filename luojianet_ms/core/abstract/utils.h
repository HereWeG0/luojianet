/**
 * This is the C++ adaptation and derivative work of Myia (https://github.com/mila-iqia/myia/).
 *
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

#ifndef LUOJIANET_MS_CORE_ABSTRACT_UTILS_H_
#define LUOJIANET_MS_CORE_ABSTRACT_UTILS_H_

#include <vector>
#include <utility>
#include <memory>
#include <string>
#include <functional>
#include "abstract/abstract_value.h"
#include "utils/any.h"
#include "utils/misc.h"
#include "utils/shape_utils.h"

namespace luojianet_ms {
namespace abstract {
ValuePtr ValueJoin(const ValuePtr &value1, const ValuePtr &value2);
TypePtr TypeJoin(const TypePtr &type1, const TypePtr &type2);
ShapePtr ShapeJoin(const ShapePtr &shape1, const ShapePtr &shape2);

AbstractBasePtr AbstractJoin(const AbstractBasePtrList &args_spec_list);
AbstractBasePtrList AbstractJoin(const AbstractBasePtrList &spec1, const AbstractBasePtrList &spec2);

// Return an abstract value for the sensitivity of x.
// The sensitivity of a function is an Env
// The sensitivity of J(x) is x
// else self.Clone;
AbstractBasePtr SensitivityTransform(const AbstractBasePtr &spec);

ShapeVector BroadcastShape(ShapeVector shpx, ShapeVector shpy);

MS_CORE_API size_t TypeIdSize(const TypeId data_type);
template <typename T>
T ShapeSize(const std::vector<T> &shape) {
  return std::accumulate(shape.begin(), shape.end(), static_cast<T>(1), std::multiplies<T>());
}

// Check dynamic shape routine
void CheckMinMaxShape(const ShapeVector &shape, ShapeVector *min_shape, ShapeVector *max_shape);

AbstractBasePtr MakeAbstract(const BaseShapePtr &base_shape, const TypePtr &type);
AbstractBasePtr MakeMonadAbstract(const MonadTypePtr &type);
AbstractBasePtr MakeAbstractTensor(const ShapePtr &shape, const TypePtr &type);
}  // namespace abstract
}  // namespace luojianet_ms
#endif  // LUOJIANET_MS_CORE_ABSTRACT_UTILS_H_
