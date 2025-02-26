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
#include "nnacl/pad_parameter.h"
using luojianet_ms::schema::PrimitiveType_PadFusion;

namespace luojianet_ms {
namespace lite {
OpParameter *PopulatePadParameter(const void *prim) {
  MS_CHECK_TRUE_RET(prim != nullptr, nullptr);
  auto primitive = static_cast<const schema::Primitive *>(prim);
  auto value = primitive->value_as_PadFusion();
  if (value == nullptr) {
    MS_LOG(ERROR) << "value is nullptr";
    return nullptr;
  }

  auto *param = reinterpret_cast<PadParameter *>(malloc(sizeof(PadParameter)));
  if (param == nullptr) {
    MS_LOG(ERROR) << "malloc PadParameter failed.";
    return nullptr;
  }
  memset(param, 0, sizeof(PadParameter));

  param->op_parameter_.type_ = primitive->value_type();
  param->pad_mode_ = value->padding_mode();
  param->constant_value_ = value->constant_value();
  return reinterpret_cast<OpParameter *>(param);
}
REG_POPULATE(PrimitiveType_PadFusion, PopulatePadParameter, SCHEMA_CUR)
}  // namespace lite
}  // namespace luojianet_ms
