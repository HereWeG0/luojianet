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

#ifndef LUOJIANET_MS_CCSRC_BACKEND_KERNEL_COMPILER_GPU_CUDA_IMPL_TILE_IMPL_CUH_
#define LUOJIANET_MS_CCSRC_BACKEND_KERNEL_COMPILER_GPU_CUDA_IMPL_TILE_IMPL_CUH_

#define TILE_MAX_DIMENSION 100
#include "runtime/device/gpu/cuda_common.h"

template <typename T>
void CalTile(const size_t output_size, const size_t input_size, const size_t shape_size, const size_t *input_shape,
             const size_t *output_shape, const T *input, T *output, cudaStream_t cuda_stream);

#endif  // LUOJIANET_MS_CCSRC_BACKEND_KERNEL_COMPILER_GPU_CUDA_IMPL_TILE_IMPL_CUH_
