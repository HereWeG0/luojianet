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
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef LUOJIANET_MS_CCSRC_PROFILER_DEVICE_ASCEND_PROFILING_H
#define LUOJIANET_MS_CCSRC_PROFILER_DEVICE_ASCEND_PROFILING_H
#include <string>
#include <memory>
#include "profiler/device/profiling.h"
#include "acl/acl_prof.h"

namespace luojianet_ms {
namespace profiler {
namespace ascend {
class AscendProfiler : public Profiler {
 public:
  static std::shared_ptr<AscendProfiler> &GetInstance();
  AscendProfiler() : profiling_options_("") {}
  ~AscendProfiler() = default;
  AscendProfiler(const AscendProfiler &) = delete;
  AscendProfiler &operator=(const AscendProfiler &) = delete;
  void Init(const std::string &profileDataPath) { return; }
  void InitProfiling(const std::string &profiling_path, uint32_t device_id, const std::string &profiling_options);
  void Stop();
  void StepProfilingEnable(const bool enable_flag) override;
  void OpDataProducerEnd() { return; }
  void Start();
  bool GetProfilingEnableFlag() const { return enable_flag_; }
  std::string GetProfilingOptions() const { return profiling_options_; }
  void SaveProfileData() { return; }
  void ClearInst() { return; }
  uint64_t GetOptionsMask() const;
  aclprofAicoreMetrics GetAicMetrics() const;
  void Finalize() const;
  bool IsInitialized() { return init_flag_; }
  void ReportErrorMessage() const;

 private:
  static std::shared_ptr<AscendProfiler> ascend_profiler_;
  std::string profiling_options_;
  uint32_t device_id_;
  aclprofConfig *acl_config_;
};
}  // namespace ascend
}  // namespace profiler
}  // namespace luojianet_ms
#endif
