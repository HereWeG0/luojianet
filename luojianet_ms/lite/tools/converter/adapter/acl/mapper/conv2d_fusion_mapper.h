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

#ifndef ACL_MAPPER_PRIMITIVE_CONV2DFUSION_MAPPER_H
#define ACL_MAPPER_PRIMITIVE_CONV2DFUSION_MAPPER_H

#include "tools/converter/adapter/acl/mapper/primitive_mapper.h"
#include "tools/converter/adapter/acl/mapper/conv_base_mapper.h"
#include "ops/fusion/conv2d_fusion.h"

namespace luojianet_ms {
namespace lite {
using luojianet_ms::ops::kNameConv2DFusion;

class Conv2DFusionMapper : public ConvBaseMapper {
 public:
  Conv2DFusionMapper() : ConvBaseMapper(kNameConv2DFusion) {}
  ~Conv2DFusionMapper() override = default;

  STATUS Mapper(const CNodePtr &cnode) override;
};
}  // namespace lite
}  // namespace luojianet_ms
#endif  // ACL_MAPPER_PRIMITIVE_CONV2DFUSION_MAPPER_H
