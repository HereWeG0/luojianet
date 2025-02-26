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

#ifndef LUOJIANET_MS_CCSRC_RUNTIME_DEVICE_ASCEND_GE_RUNTIME_TASK_TBE_TASK_H_
#define LUOJIANET_MS_CCSRC_RUNTIME_DEVICE_ASCEND_GE_RUNTIME_TASK_TBE_TASK_H_

#include <string>
#include <memory>
#include "runtime/device/ascend/ge_runtime/task/task.h"

namespace luojianet_ms::ge::model_runner {
class TbeTask : public TaskRepeater<TbeTaskInfo> {
 public:
  TbeTask(const ModelContext &model_context, const std::shared_ptr<TbeTaskInfo> &task_info);

  ~TbeTask() override;

  void Distribute() override;

  void *Args() override { return args_; }

  std::string task_name() const override { return task_info_->op_name(); }

 private:
  std::shared_ptr<TbeTaskInfo> task_info_;
  void *stream_;
  void *stub_func_;
  void *args_;
};
}  // namespace luojianet_ms::ge::model_runner
#endif  // LUOJIANET_MS_CCSRC_RUNTIME_DEVICE_ASCEND_GE_RUNTIME_TASK_TBE_TASK_H_
