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
#ifndef LUOJIANET_MS_CCSRC_BACKEND_OPTIMIZER_COMMON_PASS_MANAGER_H_
#define LUOJIANET_MS_CCSRC_BACKEND_OPTIMIZER_COMMON_PASS_MANAGER_H_

#include <utility>
#include <vector>
#include <string>
#include <memory>
#include <map>

#include "backend/optimizer/common/pass.h"
#include "backend/optimizer/common/node_pass.h"

namespace luojianet_ms {
namespace opt {
class CacheManager {
 public:
  CacheManager() {}
  ~CacheManager() = default;
  void Update(const AnfNodePtr &node);
  TypeId GetOutputType(const AnfNodePtr &node, size_t index);
  std::vector<size_t> GetOutputShape(const AnfNodePtr &node, size_t index);

 private:
  std::map<AnfNodePtr, std::map<size_t, TypeId>> type_map_;
  std::map<AnfNodePtr, std::map<size_t, std::vector<size_t>>> shape_map_;
};
using CacheManagerPtr = std::shared_ptr<CacheManager>;

// @brief For optimization passes management
class PassManager {
 public:
  explicit PassManager(const std::string &name = "pm", bool run_only_once = true)
      : name_(name), passes_{}, run_only_once_(run_only_once), cache_manager_(std::make_shared<CacheManager>()) {}
  virtual ~PassManager() = default;
  // Get all the passes added by AddPass
  const std::vector<PassPtr> &Passes() const;
  // Add graph pass, the pass object will be freed when pass manager freed.
  virtual void AddPass(const PassPtr &pass);
  // Run passes added in pass manager on the input graph
  // @param [in out] graph The graph to be optimized
  // @return true, graph changed
  // @return false, graph not changed
  virtual bool Run(const FuncGraphPtr &func_graph) const;
  // Run the given graph passes on the input graph
  // @param [in out] graph The graph to be optimized
  // @param [in] passes The given graph passes
  // @return true, graph changed
  // @return false, graph not changed
  virtual bool Run(const FuncGraphPtr &func_graph, const std::vector<PassPtr> &passes) const;
  std::string name() const { return name_; }

 protected:
  virtual bool RunPass(const FuncGraphPtr &func_graph, size_t pass_id, const PassPtr &pass) const;
  virtual std::string GetPassFullname(size_t pass_id, const PassPtr &pass) const;
  virtual void DumpPassIR(const FuncGraphPtr &func_graph, const std::string &pass_fullname) const;

  const std::string name_;
  std::vector<PassPtr> passes_;
  bool run_only_once_;
  CacheManagerPtr cache_manager_;
};
using PassManagerPtr = std::shared_ptr<PassManager>;
}  // namespace opt
}  // namespace luojianet_ms

#endif  // LUOJIANET_MS_CCSRC_BACKEND_OPTIMIZER_COMMON_PASS_MANAGER_H_
