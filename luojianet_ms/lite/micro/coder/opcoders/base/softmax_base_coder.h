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

#ifndef LUOJIANET_MS_LITE_MICRO_CODER_OPCODERS_BASE_SOFTMAX_BASE_CODER_H_
#define LUOJIANET_MS_LITE_MICRO_CODER_OPCODERS_BASE_SOFTMAX_BASE_CODER_H_

#include <vector>
#include <string>
#include "coder/opcoders/op_coder.h"
#include "nnacl/softmax_parameter.h"
#include "nnacl/int8/quantize.h"

namespace luojianet_ms::lite::micro {

class SoftmaxBaseCoder : public OperatorCoder {
 public:
  SoftmaxBaseCoder(const std::vector<Tensor *> &in_tensors, const std::vector<Tensor *> &out_tensors,
                   const Model::Node *node, size_t node_index, Target target)
      : OperatorCoder(in_tensors, out_tensors, node, node_index, target) {}

  ~SoftmaxBaseCoder() override { softmax_param_ = nullptr; }

 protected:
  int Init();

  int ReSize();

  SoftmaxParameter *softmax_param_{nullptr};

  int thread_count_{0};

  SoftmaxQuantArg quant_params_{};
};
}  // namespace luojianet_ms::lite::micro
#endif  // LUOJIANET_MS_LITE_MICRO_CODER_OPCODERS_BASE_SOFTMAX_BASE_CODER_H_
