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

#ifndef LUOJIANET_MS_CCSRC_RUNTIME_FRAMEWORK_ACTOR_OUTPUT_ACTOR_H_
#define LUOJIANET_MS_CCSRC_RUNTIME_FRAMEWORK_ACTOR_OUTPUT_ACTOR_H_

#include <vector>
#include <string>
#include <memory>
#include <utility>
#include <algorithm>
#include <map>
#include "utils/hash_map.h"
#include "runtime/framework/control_node_parser.h"
#include "runtime/framework/device_tensor_store.h"
#include "runtime/framework/actor/actor_common.h"
#include "runtime/framework/actor/abstract_actor.h"
#include "runtime/hardware/device_context.h"
#include "backend/session/anf_runtime_algorithm.h"
#include "ir/tensor.h"

namespace luojianet_ms {
namespace runtime {
using luojianet_ms::device::DeviceContext;
using luojianet_ms::session::KernelWithIndex;
using luojianet_ms::tensor::TensorPtr;

// The output actor is used to receive the output result of actor which represents the graph output.
class OutputActor : public AbstractActor {
 public:
  OutputActor(std::string name, size_t loop_count, size_t outputs_num)
      : AbstractActor(name, KernelTransformType::kOutputActor, nullptr),
        loop_count_(loop_count),
        current_count_(0),
        outputs_num_(outputs_num),
        current_outputs_num_(0) {
    outputs_.resize(outputs_num);
    output_nodes_.resize(outputs_num);
    output_device_tensors_.resize(outputs_num);
    device_contexts_.resize(outputs_num);
  }
  ~OutputActor() override = default;

  void Init() override;

  // The output actor collects loop count when receive the input control of loop count actor.
  void RunOpControl(AID *const input_control, OpContext<DeviceTensor> *const context) override;

  // The output actor collects output result when receive the data of actor.
  void RunOpData(OpData<DeviceTensor> *const input_data, OpContext<DeviceTensor> *const context) override;

  // The graph output need be set new device address every step or loop, to avoid that the device address
  // context of tensor be rewritten in the next step or next loop.
  void UpdateOutputDeviceAddress();

  // Get the member.
  size_t loop_count() const { return loop_count_; }
  size_t outputs_num() const { return outputs_num_; }
  std::vector<TensorPtr> &outputs() { return outputs_; }

 private:
  friend class GraphScheduler;
  friend class ControlNodeScheduler;

  TensorPtr CreateOutputTensor(const AnfNodePtr &output_node, size_t output_index, size_t output_position);

  // The loop count is constant, the current count is increased after each step running finished.
  // Collect the output result in the last loop which is represented by "loop_count_ - current_count_ == 1".
  size_t loop_count_;
  size_t current_count_;

  // The outputs.
  std::vector<TensorPtr> outputs_;
  std::vector<KernelWithIndex> output_nodes_;
  std::vector<DeviceTensor *> output_device_tensors_;
  size_t outputs_num_;
  size_t current_outputs_num_;

  std::map<KernelWithIndex, DeviceTensorPtr> output_node_to_tensor_device_address_;
};

using OutputActorPtr = std::shared_ptr<OutputActor>;
}  // namespace runtime
}  // namespace luojianet_ms

#endif  // LUOJIANET_MS_CCSRC_RUNTIME_FRAMEWORK_ACTOR_OUTPUT_ACTOR_H_
