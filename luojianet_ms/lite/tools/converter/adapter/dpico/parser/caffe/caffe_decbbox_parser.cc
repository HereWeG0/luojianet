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

#include "parser/caffe/caffe_decbbox_parser.h"
#include <memory>
#include <vector>
#include <map>
#include <string>
#include "common/op_attr.h"
#include "parser/caffe/caffe_detection_output_parser.h"
#include "parser/detection_output_param_holder.h"
#include "ops/custom.h"

namespace luojianet_ms {
namespace lite {
ops::PrimitiveC *CaffeDecBBoxParser::Parse(const caffe::LayerParameter &proto, const caffe::LayerParameter &weight) {
  auto prim = std::make_unique<ops::Custom>();
  if (prim == nullptr) {
    MS_LOG(ERROR) << "prim is nullptr.";
    return nullptr;
  }
  prim->set_type("DecBBox");

  if (proto.has_num_anchors()) {
    prim->AddAttr(dpico::kNumAnchors, MakeValue<uint32_t>(proto.num_anchors()));
  }
  if (proto.has_num_bboxes_per_grid()) {
    prim->AddAttr(dpico::kNumBboxesPerGrid, MakeValue<uint32_t>(proto.num_bboxes_per_grid()));
  }
  if (proto.has_num_coords()) {
    prim->AddAttr(dpico::kNumCoords, MakeValue<uint32_t>(proto.num_coords()));
  }
  if (proto.has_num_classes()) {
    auto num_classes = proto.num_classes();
    prim->AddAttr(dpico::kNumClasses, MakeValue<uint32_t>(num_classes));
    std::map<std::string, std::vector<uint8_t>> custom_attrs;
    std::vector<uint8_t> num_classes_attr(sizeof(uint32_t));
    if (memcpy_s(num_classes_attr.data(), num_classes_attr.size() * sizeof(uint8_t), &num_classes, sizeof(uint32_t)) !=
        EOK) {
      MS_LOG(ERROR) << "memcpy_s failed.";
      return nullptr;
    }
    custom_attrs[dpico::kNumClasses] = num_classes_attr;
    prim->set_attr(custom_attrs);
  }
  if (proto.has_num_grids_height()) {
    prim->AddAttr(dpico::kNumGridsHeight, MakeValue<uint32_t>(proto.num_grids_height()));
  }
  if (proto.has_num_grids_width()) {
    prim->AddAttr(dpico::kNumGridsWidth, MakeValue<uint32_t>(proto.num_grids_width()));
  }

  const auto &decbbox_param = proto.decbbox_param();
  mapper::DetectionOutputParam param;
  if (SetParamType(&param, decbbox_param.param_type()) != RET_OK) {
    MS_LOG(ERROR) << "Set param type failed.";
    return nullptr;
  }
  if (SetCodeType(&param, decbbox_param.code_type()) != RET_OK) {
    MS_LOG(ERROR) << "Set code type failed.";
    return nullptr;
  }
  (void)SetParamAttr(&param, decbbox_param);
  auto param_holder_ptr = std::make_shared<DetectionOutputParamHolder>(param);
  if (param_holder_ptr == nullptr) {
    MS_LOG(ERROR) << "new DetectionOutputParamHolder failed.";
    return nullptr;
  }

  prim->AddAttr(dpico::kDecBBoxParam, MakeValue(param_holder_ptr));
  return prim.release();
}

CaffeNodeRegistrar g_caffeDecBBoxParser("DecBBox", new CaffeDecBBoxParser());
}  // namespace lite
}  // namespace luojianet_ms
