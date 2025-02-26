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
#ifndef LUOJIANET_MS_NNACL_FP16_TOPK_FP16_H_
#define LUOJIANET_MS_NNACL_FP16_TOPK_FP16_H_

#include "nnacl/fp32/topk_fp32.h"
#include "nnacl/op_base.h"

typedef struct TopkFp16Node {
  float16_t element;
  int32_t index;
} TopkFp16Node;

#ifdef __cplusplus
extern "C" {
#endif
void TopkFp16(float16_t *input_data, float16_t *output_data, int32_t *output_index, TopkParameter *parameter);
#ifdef __cplusplus
}
#endif

#endif  // LUOJIANET_MS_NNACL_FP16_TOPK_FP16_H_
