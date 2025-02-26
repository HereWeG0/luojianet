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
#include "nnacl/reverse_sequence_parameter.h"
using luojianet_ms::schema::PrimitiveType_ReverseSequence;

namespace luojianet_ms {
namespace lite {
OpParameter *PopulateReverseSequenceParameter(const void *prim) {
  MS_CHECK_TRUE_RET(prim != nullptr, nullptr);
  auto primitive = static_cast<const schema::Primitive *>(prim);
  auto value = primitive->value_as_ReverseSequence();
  if (value == nullptr) {
    MS_LOG(ERROR) << "value is nullptr";
    return nullptr;
  }

  auto *param = reinterpret_cast<ReverseSequenceParameter *>(malloc(sizeof(ReverseSequenceParameter)));
  if (param == nullptr) {
    MS_LOG(ERROR) << "malloc ReverseSequenceParameter failed.";
    return nullptr;
  }
  memset(param, 0, sizeof(ReverseSequenceParameter));

  param->op_parameter_.type_ = primitive->value_type();
  param->seq_axis_ = static_cast<int>(value->seq_dim());
  param->batch_axis_ = static_cast<int>(value->batch_dim());
  return reinterpret_cast<OpParameter *>(param);
}
REG_POPULATE(PrimitiveType_ReverseSequence, PopulateReverseSequenceParameter, SCHEMA_CUR);
}  // namespace lite
}  // namespace luojianet_ms
