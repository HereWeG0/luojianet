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

#include "backend/kernel_compiler/akg/gpu/akg_gpu_kernel_build.h"
#include <Python.h>
#include <vector>
#include <memory>
#include <string>
#include "backend/kernel_compiler/common_utils.h"
#include "backend/kernel_compiler/akg/gpu/akg_gpu_kernel_mod.h"
#include "utils/ms_utils.h"
#include "backend/session/anf_runtime_algorithm.h"

namespace luojianet_ms {
namespace kernel {
constexpr int32_t ARGS_SIZE = 1;

void AkgGpuKernelBuilder::AkgSetKernelMod(const KernelPackPtr &kernel_pack,
                                          const AkgKernelJsonGenerator &json_generator, const AnfNodePtr &anf_node) {
  auto kernel_mod_ptr = std::make_shared<GpuKernelMod>(kernel_pack);
  auto kernel_json_info = kernel_pack->kernel_json_info();
  kernel_mod_ptr->SetInputSizeList(json_generator.input_size_list());
  kernel_mod_ptr->SetOutputSizeList(json_generator.output_size_list());
  kernel_mod_ptr->SetWorkspaceSizeList(kernel_json_info.workspaces);
  AnfAlgo::SetKernelMod(kernel_mod_ptr, anf_node.get());
}

void AkgGpuKernelBuilder::AkgSaveJsonInfo(const string &kernel_name, const string &kernel_json) {
  kernel::SaveJsonInfo(kernel_name, kernel_json, kernel::KernelMeta::GetInstance()->kernel_meta_path());
}
}  // namespace kernel
}  // namespace luojianet_ms
