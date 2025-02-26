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

#ifndef LUOJIANET_MS_CCSRC_BACKEND_KERNEL_COMPILER_GPU_CUDA_IMPL_EMBEDDING_LOOKUP_IMPL_CUH_
#define LUOJIANET_MS_CCSRC_BACKEND_KERNEL_COMPILER_GPU_CUDA_IMPL_EMBEDDING_LOOKUP_IMPL_CUH_

template <typename T, typename S>
void CalEmbeddingLookup(T *input, S *indices, T *output, size_t output_dim0, size_t output_dim1, size_t output_dim2,
                        size_t input_dim1, int64_t offset, cudaStream_t stream);

#endif  // LUOJIANET_MS_CCSRC_BACKEND_KERNEL_COMPILER_GPU_CUDA_IMPL_EMBEDDING_LOOKUP_IMPL_CUH_
