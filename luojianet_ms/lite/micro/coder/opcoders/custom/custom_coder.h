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
#ifndef LUOJIANET_MS_LITE_MICRO_CODER_OPCODERS_CUSTOM_CODER_H
#define LUOJIANET_MS_LITE_MICRO_CODER_OPCODERS_CUSTOM_CODER_H

#include <vector>
#include <string>
#include <map>
#include "coder/opcoders/op_coder.h"

namespace luojianet_ms::lite::micro {
class CustomCoder final : public OperatorCoder {
 public:
  CustomCoder(const std::vector<Tensor *> &in_tensors, const std::vector<Tensor *> &out_tensors,
              const Model::Node *node, size_t node_index, Target target)
      : OperatorCoder(in_tensors, out_tensors, node, node_index, target) {}

  ~CustomCoder() override = default;

  int Prepare(CoderContext *const context) override;

  int DoCode(CoderContext *const context) override;

 private:
  void Populate(const void *prim);
  int TransformTensors(Serializer *code, std::string array_name, const std::vector<Tensor *> &tensors);
  int TransformParams(Serializer *code, std::string var_name);
  void FreeParams(Serializer *code, std::string var_name);
  void FreeTensors(Serializer *code, std::string array_name, size_t tensors_num);

  std::string type_;
  std::map<std::string, std::string> attrs_;
  static std::map<Tensor *, void *> const_tensor_map_;
};
}  // namespace luojianet_ms::lite::micro
#endif  // LUOJIANET_MS_LITE_MICRO_CODER_OPCODERS_CUSTOM_CODER_H
