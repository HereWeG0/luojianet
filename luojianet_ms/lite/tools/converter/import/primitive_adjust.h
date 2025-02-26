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

#ifndef LUOJIANET_MS_LITE_TOOLS_OPTIMIZER_GRAPH_PRIMITIVE_ADJUST_PASS_H
#define LUOJIANET_MS_LITE_TOOLS_OPTIMIZER_GRAPH_PRIMITIVE_ADJUST_PASS_H

#include <map>
#include <string>
#include <vector>
#include "backend/optimizer/common/pass.h"
#include "tools/converter/converter_flags.h"
#include "tools/optimizer/common/gllo_utils.h"

using luojianet_ms::converter::FmkType;
namespace luojianet_ms {
namespace lite {
typedef int (*PrimitiveAdjustCreator)(const CNodePtr &value_node);
class PrimitiveAdjustRegistry {
 public:
  static PrimitiveAdjustRegistry *GetInstance() {
    static PrimitiveAdjustRegistry registry;
    return &registry;
  }

  void InsertPrimitiveAdjustMap(const std::string &key, PrimitiveAdjustCreator creator) {
    primitive_adjust_creators_[key] = creator;
  }

  PrimitiveAdjustCreator GetPrimitiveCreator(const std::string &key) {
    if (primitive_adjust_creators_.find(key) != primitive_adjust_creators_.end()) {
      return primitive_adjust_creators_[key];
    } else {
      MS_LOG(DEBUG) << "Unsupported primitive type : " << key;
      return nullptr;
    }
  }

 protected:
  std::map<std::string, PrimitiveAdjustCreator> primitive_adjust_creators_;
};

class RegistryPrimitiveAdjust {
 public:
  RegistryPrimitiveAdjust(const std::string &key, PrimitiveAdjustCreator creator) {
    PrimitiveAdjustRegistry::GetInstance()->InsertPrimitiveAdjustMap(key, creator);
  }
  virtual ~RegistryPrimitiveAdjust() = default;
};

#define REGIST_PRIMITIVE_ADJUST(type, primitive_adjust_func) \
  RegistryPrimitiveAdjust g_##type##_primitive_adjust(type, primitive_adjust_func);

class PrimitiveAdjust {
 public:
  PrimitiveAdjust() = default;
  ~PrimitiveAdjust() = default;
  void SetFmkType(FmkType fmk_type) { fmk_type_ = fmk_type; }
  bool Run(const FuncGraphPtr &func_graph);

 protected:
  FmkType fmk_type_ = FmkType::kFmkTypeMs;
};
}  // namespace lite
}  // namespace luojianet_ms

#endif  // LUOJIANET_MS_LITE_TOOLS_OPTIMIZER_GRAPH_PRIMITIVE_ADJUST_PASS_H
