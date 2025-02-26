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

#include "tools/converter/optimizer_manager.h"
#include <map>
#include <set>
#include <string>
#include <vector>
#include "backend/optimizer/common/pass.h"
#include "src/common/log_util.h"
#include "include/registry/pass_base.h"

namespace luojianet_ms {
namespace lite {
std::map<std::string, opt::PassPtr> PassStorage::pass_storage_;
std::set<std::string> PassStorage::inaccessible_for_outer_;
bool RunOptimizerPass(const FuncGraphPtr &func_graph, const std::vector<std::string> &pass_names) {
  if (func_graph == nullptr) {
    MS_LOG(ERROR) << "func graph is nullptr.";
    return false;
  }
  for (auto &pass_name : pass_names) {
    auto pass_outer = registry::PassRegistry::GetPassFromStoreRoom(pass_name);
    if (pass_outer != nullptr) {
      if (!pass_outer->Execute(func_graph)) {
        MS_LOG(WARNING) << "run pass failed, pass name is " << pass_name;
        return false;
      }
      continue;
    }
    auto pass_builtin = PassStorage::GetPassFromStorage(pass_name);
    if (pass_builtin == nullptr) {
      MS_LOG(ERROR) << "exited pass cannot be obtained, pass name is " << pass_name;
      return false;
    }
    if (!pass_builtin->Run(func_graph)) {
      MS_LOG(ERROR) << "run pass failed, pass name is " << pass_name;
      return false;
    }
  }
  return true;
}

bool RunExternalPass(const FuncGraphPtr &func_graph, registry::PassPosition position) {
  if (func_graph == nullptr) {
    MS_LOG(ERROR) << "func graph is nullptr.";
    return false;
  }
  auto schedule_task = registry::PassRegistry::GetOuterScheduleTask(position);
  for (const auto &pass_name : schedule_task) {
    if (!PassStorage::IsAccessibleForOuter(pass_name)) {
      MS_LOG(ERROR) << pass_name << " is an inaccessible pass for outer calling.";
      return false;
    }
  }
  if (!RunOptimizerPass(func_graph, schedule_task)) {
    MS_LOG(WARNING) << "run external scheduled task failed.";
    return false;
  }
  return true;
}
}  // namespace lite
}  // namespace luojianet_ms
