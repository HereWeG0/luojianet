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
#ifndef LUOJIANET_MS_LITE_SRC_RUNTIME_DELEGATE_NPU_OP_DECONVOLUTION_NPU_H_
#define LUOJIANET_MS_LITE_SRC_RUNTIME_DELEGATE_NPU_OP_DECONVOLUTION_NPU_H_

#include <vector>
#include <string>
#include <memory>
#include "include/graph/op/all_ops.h"
#include "src/delegate/npu/op/convolution_base_npu.h"

namespace luojianet_ms {
class DeconvolutionNPUOp : public ConvolutionBaseNPUOp {
 public:
  DeconvolutionNPUOp(const schema::Primitive *primitive, const std::vector<luojianet_ms::MSTensor> &in_tensors,
                     const std::vector<luojianet_ms::MSTensor> &out_tensors, std::string name)
      : ConvolutionBaseNPUOp(primitive, in_tensors, out_tensors, name) {}
  ~DeconvolutionNPUOp() override;

  int IsSupport(const schema::Primitive *primitive, const std::vector<luojianet_ms::MSTensor> &in_tensors,
                const std::vector<luojianet_ms::MSTensor> &out_tensors) override;

  int Init(const schema::Primitive *primitive, const std::vector<luojianet_ms::MSTensor> &in_tensors,
           const std::vector<luojianet_ms::MSTensor> &out_tensors) override;

  int SetNPUInputs(const std::vector<luojianet_ms::MSTensor> &in_tensors,
                   const std::vector<luojianet_ms::MSTensor> &out_tensors,
                   const std::vector<ge::Operator *> &npu_inputs) override;

  ge::Operator *GetNPUOp() override;

 private:
  int SetDeconvParam(const schema::Conv2dTransposeFusion *conv_prim);
  schema::ActivationType act_type_ = schema::ActivationType_NO_ACTIVATION;
  std::vector<int32_t> out_shape_value_{};
  hiai::op::ConvTranspose *deconv_ = nullptr;
  hiai::op::Const *out_shape_ = nullptr;
};
}  // namespace luojianet_ms
#endif  // LUOJIANET_MS_LITE_SRC_RUNTIME_DELEGATE_NPU_OP_DECONVOLUTION_NPU_H_
