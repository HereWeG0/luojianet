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

#ifndef LUOJIANET_MS_CCSRC_BACKEND_KERNEL_COMPILER_AICPU_AICPU_KERNEL_META_DATA_H_
#define LUOJIANET_MS_CCSRC_BACKEND_KERNEL_COMPILER_AICPU_AICPU_KERNEL_META_DATA_H_

#include <string>
#include <vector>
#include <memory>
#include "backend/kernel_compiler/kernel_build_info.h"

namespace luojianet_ms {
namespace kernel {
void AicpuMetadataInfo(const CNodePtr &kernel_node, std::vector<std::shared_ptr<KernelBuildInfo>> *kernel_info_list);
void AicpuMetadataInfoForSpecialNodes(const CNodePtr &kernel_node,
                                      std::vector<std::shared_ptr<KernelBuildInfo>> *kernel_info_list);
}  // namespace kernel
}  // namespace luojianet_ms
#endif  // LUOJIANET_MS_CCSRC_BACKEND_KERNEL_COMPILER_AICPU_AICPU_KERNEL_META_DATA_H_
