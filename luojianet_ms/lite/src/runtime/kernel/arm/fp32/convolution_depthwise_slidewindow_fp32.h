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

#ifndef LUOJIANET_MS_LITE_SRC_RUNTIME_KERNEL_ARM_FP32_CONVOLUTION_DEPTHWISE_SLIDEWINDOW_FP32_H_
#define LUOJIANET_MS_LITE_SRC_RUNTIME_KERNEL_ARM_FP32_CONVOLUTION_DEPTHWISE_SLIDEWINDOW_FP32_H_

#include <vector>
#include "src/inner_kernel.h"
#include "src/runtime/kernel/arm/base/convolution_base.h"
#include "nnacl/fp32/conv_depthwise_fp32.h"

namespace luojianet_ms::kernel {
class ConvolutionDepthwiseSWCPUKernel : public ConvolutionBaseCPUKernel {
 public:
  ConvolutionDepthwiseSWCPUKernel(OpParameter *parameter, const std::vector<lite::Tensor *> &inputs,
                                  const std::vector<lite::Tensor *> &outputs, const lite::InnerContext *ctx)
      : ConvolutionBaseCPUKernel(parameter, inputs, outputs, ctx, inputs.at(kWeightIndex)->data(),
                                 inputs.size() == kInputSize2 ? inputs.at(kBiasIndex)->data() : nullptr) {}
  ~ConvolutionDepthwiseSWCPUKernel() override;

  int Prepare() override;
  int ReSize() override;
  int Run() override;

  int Execute(int task_id);

 private:
  int InitPackedInputOutput();
  void FreePackedInputOutput();
  int MallocWeightBiasData() override;
  void PackWeight() override;
  SlidingWindowParam *sliding_ = nullptr;
  float *packed_input_ = nullptr;
  float *packed_output_ = nullptr;
  bool need_align_ = false;
};
}  // namespace luojianet_ms::kernel

#endif  // LUOJIANET_MS_LITE_SRC_RUNTIME_KERNEL_ARM_FP32_CONVOLUTION_DEPTHWISE_SLIDEWINDOW_FP32_H_
