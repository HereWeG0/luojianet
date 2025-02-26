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

#ifndef LUOJIANET_MS_LITE_TOOLS_CONVERTER_OPTIMIZER_MANAGER_H
#define LUOJIANET_MS_LITE_TOOLS_CONVERTER_OPTIMIZER_MANAGER_H

#include <map>
#include <set>
#include <string>
#include <vector>
#include "backend/optimizer/common/pass.h"
#include "include/errorcode.h"
#include "include/registry/pass_registry.h"
#include "ir/func_graph.h"

namespace luojianet_ms {
namespace lite {
class PassStorage {
 public:
  static int StorePass(const std::string &pass_name, const opt::PassPtr &pass, bool access_for_outer) {
    if (registry::PassRegistry::GetPassFromStoreRoom(pass_name) != nullptr) {
      return RET_ERROR;
    }
    pass_storage_[pass_name] = pass;
    if (!access_for_outer) {
      inaccessible_for_outer_.insert(pass_name);
    }
    return RET_OK;
  }
  static opt::PassPtr GetPassFromStorage(const std::string &pass_name) { return pass_storage_[pass_name]; }
  static bool IsAccessibleForOuter(const std::string &pass_name) {
    return inaccessible_for_outer_.find(pass_name) == inaccessible_for_outer_.end();
  }

 private:
  static std::map<std::string, opt::PassPtr> pass_storage_;
  static std::set<std::string> inaccessible_for_outer_;
};

bool RunOptimizerPass(const FuncGraphPtr &func_graph, const std::vector<std::string> &pass_names);
bool RunExternalPass(const FuncGraphPtr &func_graph, registry::PassPosition position);
}  // namespace lite
}  // namespace luojianet_ms

#endif  // LUOJIANET_MS_LITE_TOOLS_CONVERTER_OPTIMIZER_MANAGER_H
