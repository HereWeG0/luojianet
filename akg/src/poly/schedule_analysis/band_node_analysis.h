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
#ifndef BAND_NODE_ANALYSIS_H
#define BAND_NODE_ANALYSIS_H

#include "poly/scop_info.h"

namespace akg {
namespace ir {
namespace poly {

class AnalyzeBandNode {
 public:
  AnalyzeBandNode(const isl::schedule &sch, ScopInfo &scop_info) : sch_(sch), scop_info_(scop_info) {
    target_ = scop_info.user_config_.GetTarget();
  }
  ~AnalyzeBandNode() = default;

  void Run();

 private:
  void CollectStmtInfo();
  void AnalyzeConvAndMatmulOp(const ProvideEntry &pe);
  void AnalyzeScheduleTreeTemplate();
  void AnalyzeOuterBandTemplate();
  void ShowBandInfo();
  void AnalyzeAxisPosition();

  void DetermineTemplateOfBand(std::unique_ptr<OuterBandNode> &bn);
  bool IsGemmTempleteInBand(std::unique_ptr<OuterBandNode> &bn);
  int GetVectorizationAxisForCpu(std::unique_ptr<OuterBandNode> &bn);
  int GetCoalescedAccessAxisForCuda(const isl::schedule_node &orig_node);
  int GetLastAxisPos(const isl::schedule_node &orig_node, std::unordered_set<std::string> skip_tensors = {});

  std::string target_;
  const isl::schedule &sch_;
  ScopInfo &scop_info_;
  std::vector<const Provide *> gemm_provides_;
  std::unordered_map<isl::id, std::pair<std::string, ReduceDirection>, isl::IslIdIslHash> stmt_info_;
};

}  // namespace poly
}  // namespace ir
}  // namespace akg
#endif  // BAND_NODE_ANALYSIS_H