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
#include "src/ops/populate/arithmetic_populate.h"
#include "src/ops/populate/populate_register.h"
using luojianet_ms::schema::PrimitiveType_BiasAddGrad;
using luojianet_ms::schema::PrimitiveType_Equal;
using luojianet_ms::schema::PrimitiveType_FloorDiv;
using luojianet_ms::schema::PrimitiveType_FloorMod;
using luojianet_ms::schema::PrimitiveType_Greater;
using luojianet_ms::schema::PrimitiveType_GreaterEqual;
using luojianet_ms::schema::PrimitiveType_Less;
using luojianet_ms::schema::PrimitiveType_LessEqual;
using luojianet_ms::schema::PrimitiveType_LogicalAnd;
using luojianet_ms::schema::PrimitiveType_LogicalOr;
using luojianet_ms::schema::PrimitiveType_Maximum;
using luojianet_ms::schema::PrimitiveType_MaximumGrad;
using luojianet_ms::schema::PrimitiveType_Minimum;
using luojianet_ms::schema::PrimitiveType_MinimumGrad;
using luojianet_ms::schema::PrimitiveType_Mod;
using luojianet_ms::schema::PrimitiveType_NotEqual;
using luojianet_ms::schema::PrimitiveType_RealDiv;
using luojianet_ms::schema::PrimitiveType_SquaredDifference;

namespace luojianet_ms {
namespace lite {
ArithmeticParameter *PopulateArithmeticCommonPara(const void *prim) {
  MS_CHECK_TRUE_RET(prim != nullptr, nullptr);
  auto *primitive = static_cast<const schema::Primitive *>(prim);
  auto *param = reinterpret_cast<ArithmeticParameter *>(malloc(sizeof(ArithmeticParameter)));
  if (param == nullptr) {
    MS_LOG(ERROR) << "malloc ArithmeticParameter failed.";
    return nullptr;
  }
  memset(param, 0, sizeof(ArithmeticParameter));

  param->op_parameter_.type_ = primitive->value_type();
  param->broadcasting_ = false;
  param->ndim_ = 0;
  param->activation_type_ = 0;
  return param;
}

OpParameter *PopulateArithmetic(const void *primitive) {
  ArithmeticParameter *param = PopulateArithmeticCommonPara(primitive);
  if (param == nullptr) {
    MS_LOG(ERROR) << "PopulateArithmeticCommonPara failed.";
    return nullptr;
  }
  return reinterpret_cast<OpParameter *>(param);
}

REG_POPULATE(PrimitiveType_MinimumGrad, PopulateArithmetic, SCHEMA_CUR)
REG_POPULATE(PrimitiveType_MaximumGrad, PopulateArithmetic, SCHEMA_CUR)
REG_POPULATE(PrimitiveType_RealDiv, PopulateArithmetic, SCHEMA_CUR)
REG_POPULATE(PrimitiveType_LogicalAnd, PopulateArithmetic, SCHEMA_CUR)
REG_POPULATE(PrimitiveType_LogicalOr, PopulateArithmetic, SCHEMA_CUR)
REG_POPULATE(PrimitiveType_Equal, PopulateArithmetic, SCHEMA_CUR)
REG_POPULATE(PrimitiveType_NotEqual, PopulateArithmetic, SCHEMA_CUR)
REG_POPULATE(PrimitiveType_Less, PopulateArithmetic, SCHEMA_CUR)
REG_POPULATE(PrimitiveType_LessEqual, PopulateArithmetic, SCHEMA_CUR)
REG_POPULATE(PrimitiveType_Greater, PopulateArithmetic, SCHEMA_CUR)
REG_POPULATE(PrimitiveType_GreaterEqual, PopulateArithmetic, SCHEMA_CUR)
REG_POPULATE(PrimitiveType_Maximum, PopulateArithmetic, SCHEMA_CUR)
REG_POPULATE(PrimitiveType_Minimum, PopulateArithmetic, SCHEMA_CUR)
REG_POPULATE(PrimitiveType_FloorDiv, PopulateArithmetic, SCHEMA_CUR)
REG_POPULATE(PrimitiveType_FloorMod, PopulateArithmetic, SCHEMA_CUR)
REG_POPULATE(PrimitiveType_Mod, PopulateArithmetic, SCHEMA_CUR)
REG_POPULATE(PrimitiveType_SquaredDifference, PopulateArithmetic, SCHEMA_CUR)
REG_POPULATE(PrimitiveType_BiasAddGrad, PopulateArithmetic, SCHEMA_CUR)
}  // namespace lite
}  // namespace luojianet_ms
