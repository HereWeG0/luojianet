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

#ifndef LUOJIANET_MS_CCSRC_KERNEL_GPU_CUDA_IMPL_UNSORTED_SEGMENT_MIN_H_
#define LUOJIANET_MS_CCSRC_KERNEL_GPU_CUDA_IMPL_UNSORTED_SEGMENT_MIN_H_

#include <cuda_runtime.h>
#include "runtime/device/gpu/cuda_common.h"

// Setting warp size to sync data across threads
#define KWARPSIZE 32
template <typename T>
void CalUnsortedSegmentMin(const T *input, const int *segment_ids, const int64_t num_segments, size_t outer_size,
                           size_t inner_size, T *output, cudaStream_t stream);
#endif  // LUOJIANET_MS_CCSRC_KERNEL_GPU_CUDA_IMPL_UNSORT_SEGMENT_MIN_H_
