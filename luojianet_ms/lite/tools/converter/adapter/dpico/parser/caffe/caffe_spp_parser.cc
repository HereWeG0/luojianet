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

#include "parser/caffe/caffe_spp_parser.h"
#include <memory>
#include <map>
#include <string>
#include <vector>
#include "common/op_attr.h"
#include "ops/custom.h"

namespace luojianet_ms {
namespace lite {
ops::PrimitiveC *CaffeSppParser::Parse(const caffe::LayerParameter &proto, const caffe::LayerParameter &weight) {
  auto prim = std::make_unique<ops::Custom>();
  if (prim == nullptr) {
    MS_LOG(ERROR) << "prim is nullptr.";
    return nullptr;
  }
  prim->set_type("Spp");

  int pool_method = 0;
  if (proto.has_spp_param()) {
    const caffe::SPPParameter &spp_param = proto.spp_param();
    if (spp_param.has_pool()) {
      pool_method = static_cast<int>(spp_param.pool());
    }
    if (spp_param.has_pyramid_height()) {
      auto pyramid_height = spp_param.pyramid_height();
      prim->AddAttr(dpico::kPyramidHeight, MakeValue<uint32_t>(pyramid_height));
      std::map<std::string, std::vector<uint8_t>> custom_attrs;
      std::vector<uint8_t> pyramid_height_attr(sizeof(uint32_t));
      if (memcpy_s(pyramid_height_attr.data(), pyramid_height_attr.size() * sizeof(uint8_t), &pyramid_height,
                   sizeof(uint32_t)) != EOK) {
        MS_LOG(ERROR) << "memcpy_s failed.";
        return nullptr;
      }
      custom_attrs[dpico::kPyramidHeight] = pyramid_height_attr;
      prim->set_attr(custom_attrs);
    } else {
      MS_LOG(ERROR) << "can't find pyramid_height attr in origin model. " << proto.name();
      return nullptr;
    }
  }
  prim->AddAttr(dpico::kPoolMethod, MakeValue(pool_method));

  return prim.release();
}

CaffeNodeRegistrar g_caffeSppParser("SPP", new CaffeSppParser());
}  // namespace lite
}  // namespace luojianet_ms
