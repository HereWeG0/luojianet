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

#ifndef LUOJIANET_MS_NNACL_FP32_BATCHNORM_H_
#define LUOJIANET_MS_NNACL_FP32_BATCHNORM_H_

#include "nnacl/batchnorm_parameter.h"

#ifdef __cplusplus
extern "C" {
#endif

void BatchNormFp32(const float *input, const float *mean, const float *variance, const BatchNormParameter *param,
                   int task_id, float *output);

void FusedBatchNormFp32(const float *input, const float *scale, const float *offset, const float *mean,
                        const float *variance, const BatchNormParameter *param, int task_id, float *output);

void FusedBatchNormFp32MeanVar(const float *input, float *run_mean, float *run_var, const BatchNormParameter *param,
                               float *save_mean, float *save_var);
#ifdef __cplusplus
}
#endif

#endif  // LUOJIANET_MS_NNACL_FUSED_BATCHNORM_H_
