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
#ifndef LUOJIANET_MS_LITE_MICRO_CODER_SESSION_H_
#define LUOJIANET_MS_LITE_MICRO_CODER_SESSION_H_

#include <map>
#include <string>
#include <vector>
#include <memory>
#include "schema/inner/model_generated.h"
#include "coder/graph.h"
#include "coder/context.h"
#include "coder/config.h"
#include "coder/allocator/allocator.h"
#include "coder/opcoders/op_coder.h"

namespace luojianet_ms::lite::micro {
class CoderSession {
 public:
  CoderSession();

  ~CoderSession();

  int Init(const std::string &model_path);

  int Build();

  int Run();

  int GenerateCode();

 private:
  OpParameter *GenParameterAndInfer(const Model::Node *node, const std::vector<lite::Tensor *> &inputs,
                                    std::vector<lite::Tensor *> *outputs) const;
  int InitOpcodersInputsAndOutputs();
  int InitTensorsRef();
  int CreateOpCoders();
  int InitCodeGraph();
  int CompileGraph();
  void EndCode();

  std::unique_ptr<CoderGraph> coder_graph_{nullptr};
  std::unique_ptr<CoderContext> context_{nullptr};
  MemoryAllocator *allocator_{nullptr};
  std::vector<std::unique_ptr<OperatorCoder>> op_coders_;
  int schema_version_ = SCHEMA_VERSION::SCHEMA_CUR;
};

std::shared_ptr<CoderSession> CreateCoderSession();

}  // namespace luojianet_ms::lite::micro
#endif  // LUOJIANET_MS_LITE_MICRO_CODER_SESSION_H_
