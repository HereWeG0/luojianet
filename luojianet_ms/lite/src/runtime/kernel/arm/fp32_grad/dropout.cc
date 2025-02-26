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

#include <random>
#include <algorithm>
#include "src/runtime/kernel/arm/fp32_grad/dropout.h"
#include "schema/model_generated.h"
#include "src/kernel_registry.h"
#include "include/errorcode.h"
#include "nnacl/fp32_grad/dropout_parameter.h"

using luojianet_ms::kernel::KERNEL_ARCH;
using luojianet_ms::lite::KernelRegistrar;
using luojianet_ms::lite::RET_ERROR;
using luojianet_ms::lite::RET_NULL_PTR;
using luojianet_ms::lite::RET_OK;
using luojianet_ms::schema::PrimitiveType_Dropout;

namespace luojianet_ms::kernel {
int DropoutCPUKernel::Prepare() {
  auto param = reinterpret_cast<DropoutParameter *>(op_parameter_);
  if (param == nullptr) {
    MS_LOG(ERROR) << "Dropout op_parameter_ nullptr";
    return RET_NULL_PTR;
  }

  if ((param->ratio_ > 1.0f) || (param->ratio_ < 0.0f)) {
    MS_LOG(ERROR) << "unsupported ratio value - Dropout ratio should be between zero to one";
    return RET_ERROR;
  }

  if (param->ratio_ >= 1.0f) {
    scale_ = 1.0f;
  } else {
    scale_ = 1. / (1. - param->ratio_);
  }
  if (!InferShapeDone()) {
    return RET_OK;
  }
  return ReSize();
}

int DropoutCPUKernel::ReSize() { return RET_OK; }

int DropoutCPUKernel::Execute(int task_id) {
  auto input_ptr = reinterpret_cast<float *>(in_tensors_.at(kInputIndex)->MutableData());
  auto output_ptr = reinterpret_cast<float *>(out_tensors_.at(kOutputIndex)->MutableData());
  auto mask = reinterpret_cast<float *>(out_tensors_.at(1)->MutableData());
  auto param = reinterpret_cast<DropoutParameter *>(op_parameter_);
  auto length = in_tensors_.at(kInputIndex)->ElementsNum();

  int stride = UP_DIV(length, thread_count_);
  int count = MSMIN(stride, length - stride * task_id);

  int start = stride * task_id;
  int end = start + count;

  if (param == nullptr) {
    MS_LOG(ERROR) << "Dropout op_parameter_ nullptr";
    return RET_NULL_PTR;
  }
  if (count > 0) {
    if (IsEval()) {
      std::copy(&(input_ptr[start]), &(input_ptr[end]), &(output_ptr[start]));
    } else {
      std::default_random_engine generator;
      std::bernoulli_distribution distribution(param->ratio_);

      for (int i = start; i < end; i++) {
        mask[i] = distribution(generator);
        output_ptr[i] = input_ptr[i] * mask[i] * scale_;
      }
    }
  }
  return RET_OK;
}

int RunDropout(void *cdata, int task_id, float lhs_scale, float rhs_scale) {
  auto dropout = reinterpret_cast<DropoutCPUKernel *>(cdata);
  auto error_code = dropout->Execute(task_id);
  if (error_code != RET_OK) {
    MS_LOG(ERROR) << "Dropout Run error task_id[" << task_id << "] error_code[" << error_code << "]";
    return RET_ERROR;
  }
  return RET_OK;
}

int DropoutCPUKernel::Run() {
  int error_code = ParallelLaunch(this->ms_context_, RunDropout, this, thread_count_);
  if (error_code != RET_OK) {
    MS_LOG(ERROR) << "Dropout function error error_code[" << error_code << "]";
    return RET_ERROR;
  }
  return RET_OK;
}

kernel::InnerKernel *CpuDropoutFp32KernelCreator(const std::vector<lite::Tensor *> &inputs,
                                                 const std::vector<lite::Tensor *> &outputs, OpParameter *opParameter,
                                                 const lite::Context *ctx, const kernel::KernelKey &desc) {
  if (opParameter == nullptr) {
    MS_LOG(ERROR) << "Dropout opParameter nullptr.";
    return nullptr;
  }
  if (desc.type != schema::PrimitiveType_Dropout) {
    MS_LOG(ERROR) << "Dropout desc type should be " << schema::PrimitiveType_Dropout << " got " << desc.type;
    return nullptr;
  }
  auto *kernel =
    new (std::nothrow) DropoutCPUKernel(opParameter, inputs, outputs, static_cast<const lite::InnerContext *>(ctx));
  if (kernel == nullptr) {
    MS_LOG(ERROR) << "Dropout new kernel failed.";
    return nullptr;
  }
  return kernel;
}

REG_KERNEL(kCPU, kNumberTypeFloat32, PrimitiveType_Dropout, CpuDropoutFp32KernelCreator)
}  // namespace luojianet_ms::kernel
