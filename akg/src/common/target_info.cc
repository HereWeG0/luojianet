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


#include "target_info.h"

#include <tvm/runtime/registry.h>
#include <tvm/packed_func_ext.h>

namespace air {

TVM_STATIC_IR_FUNCTOR(IRPrinter, vtable)
.set_dispatch<GpuMemoryInfoNode>([](const ObjectRef& node, IRPrinter *p) {
    auto* op = static_cast<const GpuMemoryInfoNode*>(node.get());
    p->stream << "mem-info("
              << "max_bytes_per_block=" << op->max_bytes_per_block << ")";
});

TVM_REGISTER_NODE_TYPE(GpuMemoryInfoNode);

GpuMemoryInfo GetGpuMemoryInfo(const std::string& scope) {
  std::string fname = "gpu.info.mem." + scope;
  const runtime::PackedFunc* f = runtime::Registry::Get(fname);
  if (f == nullptr) {
    return GpuMemoryInfo();
  } else {
    return (*f)();
  }
}

}  // namespace air
