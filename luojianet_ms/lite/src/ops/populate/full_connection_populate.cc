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
#include "nnacl/matmul_parameter.h"
using luojianet_ms::schema::PrimitiveType_FullConnection;

namespace luojianet_ms {
namespace lite {
OpParameter *PopulateFullconnectionParameter(const void *prim) {
  auto *primitive = static_cast<const schema::Primitive *>(prim);
  MS_ASSERT(primitive != nullptr);
  auto value = primitive->value_as_FullConnection();
  if (value == nullptr) {
    MS_LOG(ERROR) << "value is nullptr";
    return nullptr;
  }

  auto *param = reinterpret_cast<MatMulParameter *>(malloc(sizeof(MatMulParameter)));
  if (param == nullptr) {
    MS_LOG(ERROR) << "malloc MatMulParameter failed.";
    return nullptr;
  }
  memset(param, 0, sizeof(MatMulParameter));

  param->op_parameter_.type_ = primitive->value_type();
  param->b_transpose_ = true;
  param->a_transpose_ = false;
  param->has_bias_ = value->has_bias();
  if (value->activation_type() == schema::ActivationType_RELU) {
    param->act_type_ = ActType_Relu;
  } else if (value->activation_type() == schema::ActivationType_RELU6) {
    param->act_type_ = ActType_Relu6;
  } else {
    param->act_type_ = ActType_No;
  }
  param->axis_ = value->axis();
  param->use_axis_ = value->use_axis();
  return reinterpret_cast<OpParameter *>(param);
}
REG_POPULATE(PrimitiveType_FullConnection, PopulateFullconnectionParameter, SCHEMA_CUR)
}  // namespace lite
}  // namespace luojianet_ms
