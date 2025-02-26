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

#ifndef LUOJIANET_MS_LITE_MICRO_CODER_CMSIS_NN_CONV2D_INT8_CODER_H_
#define LUOJIANET_MS_LITE_MICRO_CODER_CMSIS_NN_CONV2D_INT8_CODER_H_

#include <string>
#include <vector>
#include "coder/opcoders/cmsis-nn/int8/conv2d_base_coder.h"
#include "nnacl/conv_parameter.h"

namespace luojianet_ms::lite::micro::cmsis {

class Conv2DInt8Coder final : public Conv2DBaseCoder {
 public:
  explicit Conv2DInt8Coder(const std::vector<Tensor *> &in_tensors, const std::vector<Tensor *> &out_tensors,
                           const Model::Node *node, size_t node_index, Target target)
      : Conv2DBaseCoder(in_tensors, out_tensors, node, node_index, target) {}
  int Prepare(CoderContext *context) override;

  int DoCode(CoderContext *ctx) override;

  ~Conv2DInt8Coder() override = default;

 private:
  enum ConvOpt { Basic = 0, Convolve_1_x_n = 1, Convolve_1x1_fast = 2 };

  void CheckSupportOptimize();

  int SetParameters();

  int InitTmpBuffer();

  uint16_t input_x_{0};
  uint16_t input_y_{0};
  uint16_t input_ch_{0};
  uint16_t input_batches_{0};
  uint16_t output_ch_{0};
  uint16_t kernel_x_{0};
  uint16_t kernel_y_{0};
  uint16_t pad_x_{0};
  uint16_t pad_y_{0};
  uint16_t stride_x_{0};
  uint16_t stride_y_{0};
  int32_t out_offset_{0};
  int32_t input_offset_{0};
  int32_t out_activation_min_{0};
  int32_t out_activation_max_{0};
  uint16_t output_x_{0};
  uint16_t output_y_{0};

  int16_t *buffer_{nullptr};
  size_t buffer_size_{0};

  ConvOpt opt_{ConvOpt::Basic};
};
}  // namespace luojianet_ms::lite::micro::cmsis

#endif  // LUOJIANET_MS_LITE_MICRO_CODER_CMSIS_NN_CONV2D_INT8_CODER_H_
