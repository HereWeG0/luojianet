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

#ifndef LUOJIANET_MS_LITE_MICRO_CODER_OPCODERS_Conv2D_3X3_INT8_CODER_H_
#define LUOJIANET_MS_LITE_MICRO_CODER_OPCODERS_Conv2D_3X3_INT8_CODER_H_
#include "coder/opcoders/base/conv2d_base_coder.h"
#include <memory>
#include <string>
#include <vector>
#include "nnacl/conv_parameter.h"

namespace luojianet_ms::lite::micro::nnacl {
class Conv2D3x3Int8Coder final : public Conv2DBaseCoder {
 public:
  Conv2D3x3Int8Coder(const std::vector<Tensor *> &in_tensors, const std::vector<Tensor *> &out_tensors,
                     const Model::Node *node, size_t node_index, Target target)
      : Conv2DBaseCoder(in_tensors, out_tensors, node, node_index, target) {}

  int Prepare(CoderContext *const context) override;

  int DoCode(CoderContext *const context) override;

  ~Conv2D3x3Int8Coder() override {
    transformed_filter_addr_ = nullptr;
    new_bias_addr_ = nullptr;
  }

 private:
  int InitWeightBias();

  void ConfigInputOutput();

  int InitTmpBuffer(CoderContext *ctx);

  int16_t *transformed_filter_addr_{nullptr};
  int32_t *new_bias_addr_{nullptr};

  int16_t *block_unit_buffer_{nullptr};
  int16_t *tile_buffer_{nullptr};
  int32_t *tmp_dst_buffer_{nullptr};
  int8_t *tmp_out_{nullptr};
  int16_t *c8_input_{nullptr};

  size_t tile_buffer_size_{0};
  size_t block_unit_buffer_size_{0};
  size_t tmp_dst_buffer_size_{0};
  size_t tmp_out_size_{0};
  size_t c8_input_size_{0};
};
}  // namespace luojianet_ms::lite::micro::nnacl
#endif  // LUOJIANET_MS_LITE_MICRO_CODER_OPCODERS_Conv2D_3X3_INT8_CODER_H_
