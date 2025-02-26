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
#include "src/common/log_adapter.h"
#include "nnacl/arithmetic_self_parameter.h"
#include "src/ops/populate/populate_register.h"
using luojianet_ms::schema::PrimitiveType_Abs;
using luojianet_ms::schema::PrimitiveType_Ceil;
using luojianet_ms::schema::PrimitiveType_Cos;
using luojianet_ms::schema::PrimitiveType_Floor;
using luojianet_ms::schema::PrimitiveType_Log;
using luojianet_ms::schema::PrimitiveType_LogGrad;
using luojianet_ms::schema::PrimitiveType_LogicalNot;
using luojianet_ms::schema::PrimitiveType_Neg;
using luojianet_ms::schema::PrimitiveType_NegGrad;
using luojianet_ms::schema::PrimitiveType_Reciprocal;
using luojianet_ms::schema::PrimitiveType_Round;
using luojianet_ms::schema::PrimitiveType_Rsqrt;
using luojianet_ms::schema::PrimitiveType_Sin;
using luojianet_ms::schema::PrimitiveType_Sqrt;
using luojianet_ms::schema::PrimitiveType_Square;

namespace luojianet_ms {
namespace lite {
OpParameter *PopulateArithmeticSelf(const void *prim) {
  MS_CHECK_TRUE_RET(prim != nullptr, nullptr);
  auto *primitive = static_cast<const schema::Primitive *>(prim);

  auto *param = reinterpret_cast<ArithmeticSelfParameter *>(malloc(sizeof(ArithmeticSelfParameter)));
  if (param == nullptr) {
    MS_LOG(ERROR) << "malloc ArithmeticSelfParameter failed.";
    return nullptr;
  }
  memset(param, 0, sizeof(ArithmeticSelfParameter));

  param->op_parameter_.type_ = primitive->value_type();
  return reinterpret_cast<OpParameter *>(param);
}

REG_POPULATE(PrimitiveType_Abs, PopulateArithmeticSelf, SCHEMA_CUR)
REG_POPULATE(PrimitiveType_Cos, PopulateArithmeticSelf, SCHEMA_CUR)
REG_POPULATE(PrimitiveType_Sin, PopulateArithmeticSelf, SCHEMA_CUR)
REG_POPULATE(PrimitiveType_Log, PopulateArithmeticSelf, SCHEMA_CUR)
REG_POPULATE(PrimitiveType_Neg, PopulateArithmeticSelf, SCHEMA_CUR)
REG_POPULATE(PrimitiveType_NegGrad, PopulateArithmeticSelf, SCHEMA_CUR)
REG_POPULATE(PrimitiveType_LogGrad, PopulateArithmeticSelf, SCHEMA_CUR)
REG_POPULATE(PrimitiveType_Sqrt, PopulateArithmeticSelf, SCHEMA_CUR)
REG_POPULATE(PrimitiveType_Square, PopulateArithmeticSelf, SCHEMA_CUR)
REG_POPULATE(PrimitiveType_Rsqrt, PopulateArithmeticSelf, SCHEMA_CUR)
REG_POPULATE(PrimitiveType_LogicalNot, PopulateArithmeticSelf, SCHEMA_CUR)
REG_POPULATE(PrimitiveType_Floor, PopulateArithmeticSelf, SCHEMA_CUR)
REG_POPULATE(PrimitiveType_Ceil, PopulateArithmeticSelf, SCHEMA_CUR)
REG_POPULATE(PrimitiveType_Round, PopulateArithmeticSelf, SCHEMA_CUR)
REG_POPULATE(PrimitiveType_Reciprocal, PopulateArithmeticSelf, SCHEMA_CUR)
}  // namespace lite
}  // namespace luojianet_ms
