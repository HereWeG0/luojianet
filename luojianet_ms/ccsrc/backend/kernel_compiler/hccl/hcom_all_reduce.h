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

#ifndef LUOJIANET_MS_CCSRC_BACKEND_KERNEL_COMPILER_HCCL_HCOM_ALL_REDUCE_H_
#define LUOJIANET_MS_CCSRC_BACKEND_KERNEL_COMPILER_HCCL_HCOM_ALL_REDUCE_H_

#include <memory>
#include <vector>
#include "backend/kernel_compiler/hccl/hccl_kernel.h"

namespace luojianet_ms {
namespace kernel {
class HcomAllReduceKernel : public HcclKernel {
 public:
  HcomAllReduceKernel() = default;
  ~HcomAllReduceKernel() override = default;

  /* Inherit from kernelmod */
  bool Launch(const std::vector<AddressPtr> &inputs, const std::vector<AddressPtr> &workspace,
              const std::vector<AddressPtr> &outputs, void *stream_ptr) override;

 private:
};

MS_HCCL_REG_KERNEL(AllReduce, HcomAllReduceKernel);
}  // namespace kernel
}  // namespace luojianet_ms

#endif
