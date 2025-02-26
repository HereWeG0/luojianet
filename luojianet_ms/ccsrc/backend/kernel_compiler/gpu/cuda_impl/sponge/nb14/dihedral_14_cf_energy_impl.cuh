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
#ifndef LUOJIANET_MS_CCSRC_KERNEL_GPU_CUDA_IMPL_SPONGE_NB14_DIHEDRAL_14_CF_ENERGY_IMPL_H
#define LUOJIANET_MS_CCSRC_KERNEL_GPU_CUDA_IMPL_SPONGE_NB14_DIHEDRAL_14_CF_ENERGY_IMPL_H

#include <curand_kernel.h>
#include "runtime/device/gpu/cuda_common.h"

void Dihedral14CFEnergy(const int dihedral_14_numbers, const int atom_numbers, const int *uint_crd_f, const int *LJtype,
                        const float *charge, float *uint_crd_with_LJ_f, const float *boxlength_f, const int *a_14,
                        const int *b_14, const float *cf_scale_factor, float *ene, cudaStream_t stream);
#endif  // LUOJIANET_MS_CCSRC_KERNEL_GPU_CUDA_IMPL_SPONGE_NB14_DIHEDRAL_14_CF_ENERGY_IMPL_H
