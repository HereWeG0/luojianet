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

#include "tools/converter/adapter/acl/mapper/arithmetic_mapper.h"
#include <memory>
#include "tools/converter/adapter/acl/mapper/primitive_mapper_register.h"
#include "src/common/log_util.h"
#include "ops/real_div.h"

namespace luojianet_ms {
namespace lite {
static const std::map<std::string, PrimitivePtr> kDivTypeMap = {{"Div", std::make_shared<ops::Div>()},
                                                                {"RealDiv", std::make_shared<ops::RealDiv>()}};

STATUS AddFusionMapper::Mapper(const CNodePtr &cnode) {
  auto dst_prim = std::make_shared<ops::Add>();
  if (MoveAttrMap(cnode, dst_prim) != RET_OK) {
    MS_LOG(ERROR) << "AddFusion mapper failed.";
    return RET_ERROR;
  }
  return RET_OK;
}

STATUS DivFusionMapper::Mapper(const CNodePtr &cnode) {
  ValueNodePtr value_node = nullptr;
  PrimitivePtr src_prim = nullptr;
  if (GetValueNodeAndPrimFromCnode(cnode, &value_node, &src_prim) != lite::RET_OK) {
    MS_LOG(ERROR) << "Get primitive from cnode failed.";
    return lite::RET_ERROR;
  }
  std::string original_name = "Div";
  auto name_ptr = src_prim->GetAttr(ops::kOriginalOpName);
  if (name_ptr != nullptr) {
    original_name = GetValue<std::string>(name_ptr);
    original_name = original_name.empty() ? "Div" : original_name;
  }
  PrimitivePtr dst_prim = nullptr;
  if (kDivTypeMap.find(original_name) != kDivTypeMap.end()) {
    dst_prim = kDivTypeMap.at(original_name);
  }
  CHECK_NULL_RETURN(dst_prim);
  dst_prim->SetAttrs(src_prim->attrs());
  value_node->set_value(dst_prim);
  return RET_OK;
}

STATUS MulFusionMapper::Mapper(const CNodePtr &cnode) {
  auto dst_prim = std::make_shared<ops::Mul>();
  if (MoveAttrMap(cnode, dst_prim) != RET_OK) {
    MS_LOG(ERROR) << "MulFusion mapper failed.";
    return RET_ERROR;
  }
  return RET_OK;
}

STATUS PowFusionMapper::Mapper(const CNodePtr &cnode) {
  auto dst_prim = std::make_shared<ops::Pow>();
  if (MoveAttrMap(cnode, dst_prim) != RET_OK) {
    MS_LOG(ERROR) << "PowFusion mapper failed.";
    return RET_ERROR;
  }
  return RET_OK;
}

STATUS SubFusionMapper::Mapper(const CNodePtr &cnode) {
  auto dst_prim = std::make_shared<ops::Sub>();
  if (MoveAttrMap(cnode, dst_prim) != RET_OK) {
    MS_LOG(ERROR) << "SubFusion mapper failed.";
    return RET_ERROR;
  }
  return RET_OK;
}

REGISTER_PRIMITIVE_MAPPER(kNameAddFusion, AddFusionMapper)
REGISTER_PRIMITIVE_MAPPER(kNameDivFusion, DivFusionMapper)
REGISTER_PRIMITIVE_MAPPER(kNameMulFusion, MulFusionMapper)
REGISTER_PRIMITIVE_MAPPER(kNamePowFusion, PowFusionMapper)
REGISTER_PRIMITIVE_MAPPER(kNameSubFusion, SubFusionMapper)
}  // namespace lite
}  // namespace luojianet_ms
