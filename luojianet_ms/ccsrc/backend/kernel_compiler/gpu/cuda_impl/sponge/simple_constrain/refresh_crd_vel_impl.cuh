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

#ifndef LUOJIANET_MS_CCSRC_KERNEL_GPU_CUDA_IMPL_SPONGE_SIMPLE_CONSTRAIN_REFRESH_CRD_VEL_IMPL_H_
#define LUOJIANET_MS_CCSRC_KERNEL_GPU_CUDA_IMPL_SPONGE_SIMPLE_CONSTRAIN_REFRESH_CRD_VEL_IMPL_H_

#include <curand_kernel.h>
#include "runtime/device/gpu/cuda_common.h"

void refreshcrdvel(int atom_numbers, float dt_inverse, float dt, float exp_gamma, float half_exp_gamma_plus_half,
                   float *test_frc_f, float *mass_inverse, float *crd_f, float *vel_f, cudaStream_t stream);
#endif  // LUOJIANET_MS_CCSRC_KERNEL_GPU_CUDA_IMPL_SPONGE_SIMPLE_CONSTRAIN_REFRESH_CRD_VEL_IMPL_H_
