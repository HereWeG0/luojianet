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

#ifndef LUOJIANET_MS_LITE_SRC_RUNTIME_KERNEL_ARM_INT8_CONVOLUTION_DEPTHWISE_3X3_INT8_H_
#define LUOJIANET_MS_LITE_SRC_RUNTIME_KERNEL_ARM_INT8_CONVOLUTION_DEPTHWISE_3X3_INT8_H_

#include <vector>
#include "src/inner_kernel.h"
#include "src/runtime/kernel/arm/base/convolution_base.h"
#include "nnacl/fp32/conv_depthwise_fp32.h"

namespace luojianet_ms::kernel {
class ConvolutionDepthwise3x3Int8CPUKernel : public ConvolutionBaseCPUKernel {
 public:
  ConvolutionDepthwise3x3Int8CPUKernel(OpParameter *parameter, const std::vector<lite::Tensor *> &inputs,
                                       const std::vector<lite::Tensor *> &outputs, const InnerContext *ctx)
      : ConvolutionBaseCPUKernel(parameter, inputs, outputs, ctx, nullptr, nullptr) {}
  ~ConvolutionDepthwise3x3Int8CPUKernel() override;

  int Prepare() override;
  int ReSize() override;
  int Run() override;

  int InitWeightBias();
  int Execute(int task_id);

 private:
  int InitBuffer();
  SlidingWindowParam *sliding_ = nullptr;
  int16_t *packed_weight_ = nullptr;
  int8_t *input_ptr_ = nullptr;
  int8_t *output_ptr_ = nullptr;
  int8_t *buffer_ = nullptr;
};
}  // namespace luojianet_ms::kernel

#endif  // LUOJIANET_MS_LITE_SRC_RUNTIME_KERNEL_ARM_INT8_CONVOLUTION_DEPTHWISE_3X3_INT8_H_
