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

#include "ops/xdivy.h"
#include <functional>
#include <algorithm>
#include <memory>
#include <string>
#include <vector>
#include <set>
#include "ops/primitive_c.h"
#include "utils/check_convert_utils.h"
#include "abstract/abstract_value.h"
#include "abstract/primitive_infer_map.h"
#include "ops/op_utils.h"
namespace luojianet_ms {
namespace ops {
namespace {
abstract::ShapePtr XdivyInferShape(const PrimitivePtr &primitive, const std::vector<AbstractBasePtr> &input_args) {
  MS_EXCEPTION_IF_NULL(primitive);
  auto prim_name = primitive->name();
  const int64_t input_num = 2;
  (void)CheckAndConvertUtils::CheckInteger("input number", SizeToLong(input_args.size()), kEqual, input_num, prim_name);
  for (const auto &item : input_args) {
    MS_EXCEPTION_IF_NULL(item);
  }
  return BroadCastInferShape(prim_name, input_args);
}
TypePtr XdivyInferType(const PrimitivePtr &primitive, const std::vector<AbstractBasePtr> &input_args) {
  MS_EXCEPTION_IF_NULL(primitive);
  auto prim_name = primitive->name();
  const int64_t input_num = 2;
  (void)CheckAndConvertUtils::CheckInteger("input number", SizeToLong(input_args.size()), kEqual, input_num, prim_name);
  MS_EXCEPTION_IF_NULL(input_args[0]);
  MS_EXCEPTION_IF_NULL(input_args[1]);
  auto x = CheckAndConvertUtils::CheckArgs<abstract::AbstractTensor>(prim_name, input_args, 0);
  auto y = CheckAndConvertUtils::CheckArgs<abstract::AbstractTensor>(prim_name, input_args, 1);
  (void)abstract::CheckDtypeSame(prim_name, x, y);
  auto input_type = input_args[0]->BuildType();
  MS_EXCEPTION_IF_NULL(input_type);
  if (!input_type->isa<TensorType>()) {
    MS_EXCEPTION(TypeError) << "The " << prim_name << "'s"
                            << " input must be tensor type but got " << input_type->ToString();
  }
  const std::set<TypePtr> valid_types = {kFloat32, kFloat16};
  (void)CheckAndConvertUtils::CheckTensorTypeValid("x", input_type, valid_types, prim_name);
  (void)CheckAndConvertUtils::CheckTensorTypeValid("y", input_type, valid_types, prim_name);
  return input_type;
}
}  // namespace
AbstractBasePtr XdivyInfer(const abstract::AnalysisEnginePtr &, const PrimitivePtr &primitive,
                           const std::vector<AbstractBasePtr> &input_args) {
  auto shape = XdivyInferShape(primitive, input_args);
  auto type = XdivyInferType(primitive, input_args);
  return abstract::MakeAbstract(shape, type);
}
REGISTER_PRIMITIVE_EVAL_IMPL(Xdivy, prim::kPrimXdivy, XdivyInfer, nullptr, true);
}  // namespace ops
}  // namespace luojianet_ms
