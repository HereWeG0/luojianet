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

#include "tools/converter/parser/caffe/caffe_activation_parser.h"
#include <memory>
#include "ops/fusion/activation.h"
#include "nnacl/op_base.h"

namespace luojianet_ms {
namespace lite {
ops::PrimitiveC *CaffeReluParser::Parse(const caffe::LayerParameter &proto, const caffe::LayerParameter &weight) {
  auto prim = std::make_unique<ops::Activation>();
  MS_CHECK_TRUE_RET(prim != nullptr, nullptr);
  prim->set_activation_type(luojianet_ms::ActivationType::RELU);

  if (proto.has_relu_param() && proto.relu_param().has_negative_slope()) {
    float negative_slope = proto.relu_param().negative_slope();
    if (negative_slope != 0) {
      prim->set_activation_type(luojianet_ms::ActivationType::LEAKY_RELU);
      prim->set_alpha(negative_slope);
    }
  }

  return prim.release();
}

ops::PrimitiveC *CaffeRelu6Parser::Parse(const caffe::LayerParameter &proto, const caffe::LayerParameter &weight) {
  auto prim = std::make_unique<ops::Activation>();
  MS_CHECK_TRUE_RET(prim != nullptr, nullptr);
  prim->set_activation_type(luojianet_ms::ActivationType::RELU6);

  return prim.release();
}

ops::PrimitiveC *CaffeSigmoidParser::Parse(const caffe::LayerParameter &proto, const caffe::LayerParameter &weight) {
  auto prim = std::make_unique<ops::Activation>();
  MS_CHECK_TRUE_RET(prim != nullptr, nullptr);
  prim->set_activation_type(luojianet_ms::ActivationType::SIGMOID);

  return prim.release();
}

ops::PrimitiveC *CaffeTanhParser::Parse(const caffe::LayerParameter &proto, const caffe::LayerParameter &weight) {
  auto prim = std::make_unique<ops::Activation>();
  MS_CHECK_TRUE_RET(prim != nullptr, nullptr);
  prim->set_activation_type(luojianet_ms::ActivationType::TANH);

  return prim.release();
}

ops::PrimitiveC *CaffeEluParser::Parse(const caffe::LayerParameter &proto, const caffe::LayerParameter &weight) {
  auto prim = std::make_unique<ops::Activation>();
  MS_CHECK_TRUE_RET(prim != nullptr, nullptr);
  prim->set_activation_type(luojianet_ms::ActivationType::ELU);

  if (proto.has_elu_param()) {
    const caffe::ELUParameter &eluParameter = proto.elu_param();
    if (eluParameter.has_alpha()) {
      prim->set_alpha(eluParameter.alpha());
    }
  }

  return prim.release();
}

CaffeNodeRegistrar g_caffeReluParser("ReLU", new CaffeReluParser());
CaffeNodeRegistrar g_caffeRelu6Parser("ReLU6", new CaffeRelu6Parser());
CaffeNodeRegistrar g_caffeSigmoidParser("Sigmoid", new CaffeSigmoidParser());
CaffeNodeRegistrar g_caffeTanhParser("TanH", new CaffeTanhParser());
CaffeNodeRegistrar g_caffeEluParser("Elu", new CaffeEluParser());
}  // namespace lite
}  // namespace luojianet_ms
