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

#include "parser/caffe/caffe_log_parser.h"
#include <memory>
#include "ops/log.h"

namespace luojianet_ms {
namespace lite {
ops::PrimitiveC *CaffeLogParser::Parse(const caffe::LayerParameter &proto, const caffe::LayerParameter &weight) {
  auto prim = std::make_unique<ops::Log>();
  if (prim == nullptr) {
    MS_LOG(ERROR) << "prim is nullptr.";
    return nullptr;
  }
  if (proto.has_log_param()) {
    const caffe::LogParameter &log_param = proto.log_param();

    if (log_param.has_base()) {
      prim->AddAttr(ops::kBase, MakeValue<float>(log_param.base()));
    }

    if (log_param.has_scale()) {
      prim->AddAttr(ops::kScale, MakeValue<float>(log_param.scale()));
    }

    if (log_param.has_shift()) {
      prim->AddAttr(ops::kShift, MakeValue<float>(log_param.shift()));
    }
  }

  return prim.release();
}

CaffeNodeRegistrar g_caffeLogParser("Log", new CaffeLogParser());
}  // namespace lite
}  // namespace luojianet_ms
