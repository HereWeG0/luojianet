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

#ifndef LUOJIANET_MS_CCSRC_RUNTIME_DEVICE_ASCEND_GE_RUNTIME_TASK_PROFILER_TASK_H_
#define LUOJIANET_MS_CCSRC_RUNTIME_DEVICE_ASCEND_GE_RUNTIME_TASK_PROFILER_TASK_H_

#include <memory>
#include "runtime/device/ascend/ge_runtime/task/task.h"

namespace luojianet_ms::ge::model_runner {
class ProfilerTask : public TaskRepeater<ProfilerTraceTaskInfo> {
 public:
  ProfilerTask(const ModelContext &model_context, const std::shared_ptr<ProfilerTraceTaskInfo> &task_info);

  ~ProfilerTask() override;

  void Distribute() override;

 private:
  std::shared_ptr<ProfilerTraceTaskInfo> task_info_;
  rtStream_t stream_;
};
}  // namespace luojianet_ms::ge::model_runner
#endif  // LUOJIANET_MS_CCSRC_RUNTIME_DEVICE_ASCEND_GE_RUNTIME_TASK_PROFILER_TASK_H_
