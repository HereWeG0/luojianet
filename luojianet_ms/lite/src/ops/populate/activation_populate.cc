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
#include "src/ops/populate/populate_register.h"
#include "nnacl/fp32/activation_fp32.h"
using luojianet_ms::schema::PrimitiveType_Activation;

namespace luojianet_ms {
namespace lite {
OpParameter *PopulateRelu6Parameter(const void *prim) {
  auto primitive = static_cast<const schema::Primitive *>(prim);
  MS_ASSERT(primitive != nullptr);
  auto value = primitive->value_as_Activation();
  if (value == nullptr) {
    MS_LOG(ERROR) << "value is nullptr";
    return nullptr;
  }

  auto *param = reinterpret_cast<ActivationParameter *>(malloc(sizeof(ActivationParameter)));
  if (param == nullptr) {
    MS_LOG(ERROR) << "malloc ActivationParameter failed.";
    return nullptr;
  }
  memset(param, 0, sizeof(ActivationParameter));

  param->op_parameter_.type_ = primitive->value_type();
  param->type_ = static_cast<int>(value->activation_type());
  param->alpha_ = value->alpha();
  param->min_val_ = value->min_val();
  param->max_val_ = value->max_val();
  return reinterpret_cast<OpParameter *>(param);
}

REG_POPULATE(PrimitiveType_Activation, PopulateRelu6Parameter, SCHEMA_CUR)
}  // namespace lite
}  // namespace luojianet_ms
