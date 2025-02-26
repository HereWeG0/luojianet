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

#include "src/runtime/kernel/arm/int8/crop_int8.h"
#include "src/kernel_registry.h"

using luojianet_ms::kernel::KERNEL_ARCH;
using luojianet_ms::lite::KernelRegistrar;
using luojianet_ms::lite::RET_ERROR;
using luojianet_ms::lite::RET_MEMORY_FAILED;
using luojianet_ms::lite::RET_OK;
using luojianet_ms::schema::PrimitiveType_Crop;

namespace luojianet_ms::kernel {
int CropInt8CPUKernel::Prepare() {
  auto ret = CropBaseCPUKernel::Prepare();
  if (ret != RET_OK) {
    return ret;
  }
  auto *input_tensor = in_tensors_.at(kInputIndex);
  auto in_quant_args = input_tensor->quant_params();
  crop_para_->quant_arg.in_args_.scale_ = in_quant_args.front().scale;
  crop_para_->quant_arg.in_args_.zp_ = in_quant_args.front().zeroPoint;

  auto *out_tensor = out_tensors_.at(kOutputIndex);
  auto out_quant_args = out_tensor->quant_params();
  crop_para_->quant_arg.out_args_.scale_ = out_quant_args.front().scale;
  crop_para_->quant_arg.out_args_.zp_ = out_quant_args.front().zeroPoint;

  crop_para_->quant_arg.output_activation_max_ = std::numeric_limits<int8_t>::max();
  crop_para_->quant_arg.output_activation_min_ = std::numeric_limits<int8_t>::min();
  if (!InferShapeDone()) {
    return RET_OK;
  }
  return ReSize();
}

int CropInt8CPUKernel::ReSize() { return CropBaseCPUKernel::ReSize(); }

int CropInt8CPUKernel::Run() {
  auto ret = ParallelLaunch(this->ms_context_, CropInt8Run, this, crop_para_->thread_count_);
  return ret;
}

int CropInt8Run(void *cdata, int task_id, float lhs_scale, float rhs_scale) {
  auto crop = reinterpret_cast<CropInt8CPUKernel *>(cdata);
  crop->DoExecute(task_id);
  return RET_OK;
}

void CropInt8CPUKernel::DoExecute(int task_id) {
  auto input_tensor = in_tensors_.at(kInputIndex);
  auto out_tensor = out_tensors_.at(kOutputIndex);
  int8_t *input_data = reinterpret_cast<int8_t *>(input_tensor->data());
  int8_t *output_data = reinterpret_cast<int8_t *>(out_tensor->data());
  Int8Crop(input_data, output_data, task_id, crop_para_);
}

REG_KERNEL(kCPU, kNumberTypeInt8, PrimitiveType_Crop, LiteKernelCreator<CropInt8CPUKernel>)
}  // namespace luojianet_ms::kernel
