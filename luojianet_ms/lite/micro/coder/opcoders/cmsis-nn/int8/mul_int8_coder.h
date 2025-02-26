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

#ifndef LUOJIANET_MS_LITE_MICRO_CODER_OPCODERS_CMSIS_NN_MUL_INT8_CODER_H_
#define LUOJIANET_MS_LITE_MICRO_CODER_OPCODERS_CMSIS_NN_MUL_INT8_CODER_H_

#include <vector>
#include "coder/opcoders/op_coder.h"

namespace luojianet_ms::lite::micro::cmsis {
class MulInt8Coder final : public OperatorCoder {
 public:
  MulInt8Coder(const std::vector<Tensor *> &in_tensors, const std::vector<Tensor *> &out_tensors,
               const Model::Node *node, size_t node_index, Target target)
      : OperatorCoder(in_tensors, out_tensors, node, node_index, target) {}
  ~MulInt8Coder() override = default;
  int Prepare(CoderContext *const context) override;

  int DoCode(CoderContext *const context) override;

 private:
  Tensor *input1_{nullptr};
  Tensor *input2_{nullptr};

  int32_t input_1_offset_{0};
  int32_t input_2_offset_{0};
  int32_t out_offset_{0};
  int32_t out_mult_{0};
  int32_t out_shift_{0};
  int32_t out_activation_min_{0};
  int32_t out_activation_max_{0};
  uint32_t block_size_{0};
};
}  // namespace luojianet_ms::lite::micro::cmsis

#endif  // LUOJIANET_MS_LITE_MICRO_CODER_OPCODERS_CMSIS_NN_MUL_INT8_CODER_H_
