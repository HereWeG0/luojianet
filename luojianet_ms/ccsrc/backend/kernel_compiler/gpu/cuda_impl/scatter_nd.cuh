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

#ifndef LUOJIANET_MS_SCATTER_ND_GPU_CU_H
#define LUOJIANET_MS_SCATTER_ND_GPU_CU_H

#include "runtime/device/gpu/cuda_common.h"

template <typename T, typename S>
void ScatterNd(S *indices, T *update, T *output, const size_t &block_size, const size_t &input_size,
               const size_t &output_size, const size_t &indices_dim_0, const size_t &indices_dim_1, S *indices_stride,
               S *work_shape, cudaStream_t stream);
#endif  // LUOJIANET_MS_SCATTER_ND_GPU_CU_H
