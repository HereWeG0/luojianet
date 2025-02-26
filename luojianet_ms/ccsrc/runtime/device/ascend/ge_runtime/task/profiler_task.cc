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

#include "runtime/device/ascend/ge_runtime/task/profiler_task.h"
#include "runtime/device/ascend/ge_runtime/task/task_factory.h"

namespace luojianet_ms::ge::model_runner {
ProfilerTask::ProfilerTask(const ModelContext &model_context, const std::shared_ptr<ProfilerTraceTaskInfo> &task_info)
    : TaskRepeater<ProfilerTraceTaskInfo>(model_context, task_info), task_info_(task_info), stream_(nullptr) {
  MS_EXCEPTION_IF_NULL(task_info);
  auto stream_list = model_context.stream_list();
  uint32_t stream_id = task_info->stream_id();
  MS_LOG(INFO) << "Stream list size: " << stream_list.size() << ", stream id: " << stream_id;
  if (stream_id >= stream_list.size()) {
    MS_LOG(EXCEPTION) << "Index: " << task_info->stream_id() << " >= stream_list.size(): " << stream_list.size();
  }
  stream_ = stream_list[stream_id];
}

ProfilerTask::~ProfilerTask() {}

void ProfilerTask::Distribute() {
  MS_LOG(INFO) << "ProfilerTask Distribute start.";
  MS_LOG(INFO) << "log id = " << task_info_->log_id() << ", notify = " << task_info_->notify()
               << ", flat = " << task_info_->flat();
  uint32_t model_id = 0;
  rtError_t rt_model_ret = rtModelGetId(model_handle_, &model_id);
  if (rt_model_ret != RT_ERROR_NONE) {
    MS_LOG(EXCEPTION) << "Call rt api rtModelGetId failed, ret: " << rt_model_ret;
  }
  uint64_t new_model_id = 0;
  new_model_id = static_cast<uint64_t>(model_id);
  uint64_t first_id = 0;
  if (task_info_->log_id() > static_cast<size_t>(std::numeric_limits<uint16_t>::max())) {
    MS_LOG(EXCEPTION) << "Invalid log id " << task_info_->log_id() << " over max uint16_t.";
  }
  rtError_t rt_ret = rtProfilerTraceEx(first_id, new_model_id, static_cast<uint16_t>(task_info_->log_id()), stream_);
  if (rt_ret != RT_ERROR_NONE) {
    MS_LOG(EXCEPTION) << "Call rt api rtProfilerTraceEx failed, ret: " << rt_ret;
  }
  MS_LOG(INFO) << "DistributeTask end.";
}

REGISTER_TASK(TaskInfoType::PROFILER_TRACE, ProfilerTask, ProfilerTraceTaskInfo);
}  // namespace luojianet_ms::ge::model_runner
