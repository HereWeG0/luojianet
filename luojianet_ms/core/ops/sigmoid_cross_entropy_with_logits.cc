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

#include "ops/sigmoid_cross_entropy_with_logits.h"
#include "ops/op_utils.h"
#include "utils/check_convert_utils.h"

namespace luojianet_ms {
namespace ops {
namespace {
abstract::ShapePtr SigmoidCrossEntropyWithLogitsInferShape(const PrimitivePtr &primitive,
                                                           const std::vector<AbstractBasePtr> &input_args) {
  MS_EXCEPTION_IF_NULL(primitive);
  auto op_name = primitive->name();
  MS_LOG(INFO) << "Do infer shape for op " << op_name;
  const int64_t kInputNum = 2;
  (void)CheckAndConvertUtils::CheckInputArgs(input_args, kGreaterEqual, kInputNum, op_name);
  auto logits_shape = input_args[0]->BuildShape();
  auto label_shape = input_args[1]->BuildShape();
  auto logits_shape_ptr = logits_shape->cast<abstract::ShapePtr>();
  auto label_shape_ptr = label_shape->cast<abstract::ShapePtr>();
  // logits and label must have the same shape when is not dynamic
  if (!logits_shape_ptr->IsDynamic() && !label_shape_ptr->IsDynamic()) {
    if (*logits_shape != *label_shape) {
      MS_EXCEPTION(ValueError) << op_name << " evaluator arg label shape " << label_shape->ToString()
                               << " are not consistent with logits shape " << logits_shape->ToString();
    }
  }
  auto logits_element = logits_shape->cast<abstract::ShapePtr>();
  MS_EXCEPTION_IF_NULL(logits_element);
  return logits_element;
}

TypePtr SigmoidCrossEntropyWithLogitsInferType(const PrimitivePtr &primitive,
                                               const std::vector<AbstractBasePtr> &input_args) {
  MS_EXCEPTION_IF_NULL(primitive);
  const int64_t kInputNum = 2;
  (void)CheckAndConvertUtils::CheckInputArgs(input_args, kGreaterEqual, kInputNum, primitive->name());
  auto logits_type = input_args[kInputIndex0]->BuildType();
  auto label_type = input_args[kInputIndex1]->BuildType();
  const std::set<TypePtr> valid_types = {kBool,   kInt,    kInt8,   kInt16, kInt32,   kInt64,   kUInt,    kUInt8,
                                         kUInt16, kUInt32, kUInt64, kFloat, kFloat16, kFloat32, kFloat64, kComplex64};
  std::map<std::string, TypePtr> args;
  (void)args.insert({"logits_type", logits_type});
  (void)args.insert({"label_type", label_type});
  (void)CheckAndConvertUtils::CheckTensorTypeSame(args, valid_types, primitive->name());
  return logits_type;
}
}  // namespace
AbstractBasePtr SigmoidCrossEntropyWithLogitsInfer(const abstract::AnalysisEnginePtr &, const PrimitivePtr &primitive,
                                                   const std::vector<AbstractBasePtr> &input_args) {
  auto infer_type = SigmoidCrossEntropyWithLogitsInferType(primitive, input_args);
  auto infer_shape = SigmoidCrossEntropyWithLogitsInferShape(primitive, input_args);
  return abstract::MakeAbstract(infer_shape, infer_type);
}
REGISTER_PRIMITIVE_EVAL_IMPL(SigmoidCrossEntropyWithLogits, prim::kPrimSigmoidCrossEntropyWithLogits,
                             SigmoidCrossEntropyWithLogitsInfer, nullptr, true);
}  // namespace ops
}  // namespace luojianet_ms
