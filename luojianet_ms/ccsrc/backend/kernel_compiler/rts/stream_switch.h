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

#ifndef LUOJIANET_MS_CCSRC_BACKEND_KERNEL_COMPILER_RTS_STREAM_SWITCH_H
#define LUOJIANET_MS_CCSRC_BACKEND_KERNEL_COMPILER_RTS_STREAM_SWITCH_H

#include <memory>
#include <vector>
#include "backend/kernel_compiler/rts/rt_kernel.h"
#include "backend/kernel_compiler/rts/rt_kernel_info.h"

namespace luojianet_ms {
namespace kernel {
class StreamSwitchKernel : public RtKernel {
 public:
  StreamSwitchKernel();
  ~StreamSwitchKernel() override;

  bool Init(const AnfNodePtr &anf_node) override;

  bool Launch(const std::vector<AddressPtr> &inputs, const std::vector<AddressPtr> &workspace,
              const std::vector<AddressPtr> &outputs, void *stream_ptr) override;
  std::vector<TaskInfoPtr> GenTask(const std::vector<AddressPtr> &inputs, const std::vector<AddressPtr> &workspace,
                                   const std::vector<AddressPtr> &outputs, uint32_t stream_id) override;

 private:
  rtCondition_t cond_;
  uint32_t true_stream_index_;
  rtSwitchDataType_t data_type_;
};

MS_REG_RTKERNEL(streamswitch, StreamSwitchKernel);
}  // namespace kernel
}  // namespace luojianet_ms

#endif  // LUOJIANET_MS_CCSRC_BACKEND_KERNEL_COMPILER_RTS_STREAM_SWITCH_H
