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

#ifndef LUOJIANET_MS_CCSRC_KERNEL_GPU_CUDA_IMPL_LAYER_NORM_H_
#define LUOJIANET_MS_CCSRC_KERNEL_GPU_CUDA_IMPL_LAYER_NORM_H_

#include "runtime/device/gpu/cuda_common.h"

template <typename T>
struct DynamicSharedMem;
template<>
struct DynamicSharedMem<float> {
    __device__ float *addr() {
        extern __shared__ float addr_float[];
        return addr_float;
    }
};
template<>
struct DynamicSharedMem<half> {
    __device__ half *addr() {
        extern __shared__ half addr_half[];
        return addr_half;
    }
};

template <typename T>
void LayerNorm(const int& outer, const int& inner, const int& param_dim, const T& epsilon, const T* x, const T* gamma,
               const T* beta, T* y, T* mean, T* var, cudaStream_t stream);

#endif  // LUOJIANET_MS_CCSRC_KERNEL_GPU_CUDA_IMPL_LAYER_NORM_H_
