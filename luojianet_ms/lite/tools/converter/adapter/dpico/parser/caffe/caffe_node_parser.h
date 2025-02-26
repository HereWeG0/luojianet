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

#ifndef DPICO_PARSER_CAFFE_NODE_PARSER_H_
#define DPICO_PARSER_CAFFE_NODE_PARSER_H_

#include <string>
#include <utility>
#include <vector>
#include <unordered_map>
#include "google/protobuf/message.h"
#include "./pico_caffe.pb.h"
#include "common/anf_util.h"
#include "utils/check_convert_utils.h"
#include "ops/primitive_c.h"

using luojianet_ms::lite::STATUS;

namespace luojianet_ms {
namespace lite {
constexpr int kNums2 = 2;
constexpr int kNums4 = 4;
class CaffeNodeParser {
 public:
  explicit CaffeNodeParser(std::string nodeName) : name(std::move(nodeName)) {}

  virtual ~CaffeNodeParser() = default;

  virtual ops::PrimitiveC *Parse(const caffe::LayerParameter &proto, const caffe::LayerParameter &weight) {
    return nullptr;
  }

 protected:
  const std::string name;
};

STATUS ConvertShape(const caffe::BlobProto &proto, std::vector<int32_t> *shape);
}  // namespace lite
}  // namespace luojianet_ms

#endif  // DPICO_PARSER_CAFFE_NODE_PARSER_H_
