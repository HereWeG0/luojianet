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

#ifndef LUOJIANET_MS_CCSRC_BACKEND_KERNEL_COMPILER_KERNEL_QUERY_H_
#define LUOJIANET_MS_CCSRC_BACKEND_KERNEL_COMPILER_KERNEL_QUERY_H_

#include <vector>
#include <string>
#include <memory>
#include "backend/kernel_compiler/kernel.h"
#include "backend/kernel_compiler/kernel_build_info.h"

namespace luojianet_ms {
namespace kernel {
void KernelQuery(const CNodePtr &kernel_node, std::vector<std::shared_ptr<kernel::KernelBuildInfo>> *kernel_info_list,
                 KernelType kernel_type = KernelType::UNKNOWN_KERNEL_TYPE);
void AICPUQuery(const CNodePtr &kernel_node, std::vector<std::shared_ptr<kernel::KernelBuildInfo>> *kernel_info_list);
bool IsSupportedByAICPU(const AnfNodePtr &kernel_node, const KernelBuildInfoPtr &select_kernel_build_info);
bool IsSupportedByAICore(const AnfNodePtr &kernel_node, const KernelBuildInfoPtr &select_kernel_build_info);
}  // namespace kernel
}  // namespace luojianet_ms
#endif  // LUOJIANET_MS_CCSRC_BACKEND_KERNEL_COMPILER_KERNEL_QUERY_H_
