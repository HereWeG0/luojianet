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

#ifndef LUOJIANET_MS_CCSRC_FRONTEND_PARALLEL_OPS_INFO_UNSORTEDSEGMENTOP_INFO_H_
#define LUOJIANET_MS_CCSRC_FRONTEND_PARALLEL_OPS_INFO_UNSORTEDSEGMENTOP_INFO_H_

#include <memory>
#include <string>
#include <vector>

#include "utils/hash_map.h"
#include "frontend/parallel/auto_parallel/operator_costmodel.h"
#include "frontend/parallel/ops_info/operator_info.h"
#include "frontend/parallel/strategy.h"
#include "ir/value.h"

namespace luojianet_ms {
namespace parallel {
constexpr size_t UNSORTEDSEGMENTOP_INPUTS_SIZE = 2;
constexpr size_t UNSORTEDSEGMENTOP_OUTPUTS_SIZE = 1;

// The operator UnsortedSegment accepts three inputs:
// input0 : vector, the shape is x1,x2,x3,...,xr
// input1 : segment id, the shape is x1,x2,..,xn
// input2 : value, the number of the segments
// For Sum:  r >= n
// For Min:  r >=n, n=1
class UnsortedSegmentOpInfo : public OperatorInfo {
 public:
  UnsortedSegmentOpInfo(const std::string &name, const Shapes &inputs_shape, const Shapes &outputs_shape,
                        const PrimitiveAttrs &attrs, OperatorCostPtr cost)
      : OperatorInfo(name, inputs_shape, outputs_shape, attrs, cost) {}
  ~UnsortedSegmentOpInfo() override = default;

  std::vector<StrategyPtr> GenerateOpStrategies(int64_t stage_id) override;
  Status SetCostUnderStrategy(const StrategyPtr &strategy) override;
  std::shared_ptr<Strategys> GenerateBatchStrategies() override;

 protected:
  Status CheckStrategy(const StrategyPtr &strategy) override;
  Status InferForwardCommunication() override;
  Status InferMirrorOps() override;
  Status InferDevMatrixShape() override;
  Status InferTensorMap() override;
  Status GetAttrs() override;

 private:
  Status ComputeReplaceGraph(const CNodePtr &cnode);
};

class UnsortedSegmentSumInfo : public UnsortedSegmentOpInfo {
 public:
  UnsortedSegmentSumInfo(const std::string &name, const Shapes &inputs_shape, const Shapes &outputs_shape,
                         const PrimitiveAttrs &attrs)
      : UnsortedSegmentOpInfo(name, inputs_shape, outputs_shape, attrs, std::make_shared<UnsortedSegmentSumCost>()) {}
  ~UnsortedSegmentSumInfo() override = default;
};

class UnsortedSegmentMinInfo : public UnsortedSegmentOpInfo {
 public:
  UnsortedSegmentMinInfo(const std::string &name, const Shapes &inputs_shape, const Shapes &outputs_shape,
                         const PrimitiveAttrs &attrs)
      : UnsortedSegmentOpInfo(name, inputs_shape, outputs_shape, attrs, std::make_shared<UnsortedSegmentMinCost>()) {}
  ~UnsortedSegmentMinInfo() override = default;

  ReplaceGraphPtr replace_graph(const CNodePtr &cnode) override;
  Status InferForwardCommunication() override { return SUCCESS; }

 protected:
  Status ComputeReplaceGraph(const CNodePtr &cnode);
};
class UnsortedSegmentMaxInfo : public UnsortedSegmentOpInfo {
 public:
  UnsortedSegmentMaxInfo(const std::string &name, const Shapes &inputs_shape, const Shapes &outputs_shape,
                         const PrimitiveAttrs &attrs)
      : UnsortedSegmentOpInfo(name, inputs_shape, outputs_shape, attrs, std::make_shared<UnsortedSegmentMaxCost>()) {}
  ~UnsortedSegmentMaxInfo() override = default;

  ReplaceGraphPtr replace_graph(const CNodePtr &cnode) override;
  Status InferForwardCommunication() override { return SUCCESS; }

 protected:
  Status ComputeReplaceGraph(const CNodePtr &cnode);
};

}  // namespace parallel
}  // namespace luojianet_ms
#endif  // LUOJIANET_MS_CCSRC_FRONTEND_PARALLEL_OPS_INFO_UNSORTEDSEGMENTOP_INFO_H_
