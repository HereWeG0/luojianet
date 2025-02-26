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

#ifndef LUOJIANET_MS_CCSRC_KERNEL_GPU_CUDA_IMPL_CROSSENTROPY_H_
#define LUOJIANET_MS_CCSRC_KERNEL_GPU_CUDA_IMPL_CROSSENTROPY_H_

#include "runtime/device/gpu/cuda_common.h"

// The batch size limit to judge whether to use multiple threads.
constexpr int kLargeBatchLowLimit = 32768;

template <typename T, typename S>
void CrossEntropyWithSparse(const T *logits, const S *labels, const size_t batch_size, const size_t class_num, T *loss,
                            cudaStream_t cuda_stream);

template <typename T, typename S>
void CrossEntropyGradWithSparse(const T *logits, const S *labels, const size_t batch_size, const size_t class_num,
                                T *grad, cudaStream_t cuda_stream);

template <typename T, typename S>
void CrossEntropy(const T *logits, const S *labels, const size_t batch_size, const size_t class_num, T *losses,
                  T *dlogits, cudaStream_t cuda_stream);
#endif  // LUOJIANET_MS_CCSRC_KERNEL_GPU_CUDA_IMPL_CROSSENTROPY_H_
