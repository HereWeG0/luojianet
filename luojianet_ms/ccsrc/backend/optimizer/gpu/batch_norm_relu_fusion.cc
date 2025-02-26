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
#include "backend/optimizer/gpu/batch_norm_relu_fusion.h"

#include <memory>
#include <vector>
#include <string>

#include "backend/session/anf_runtime_algorithm.h"
#include "ir/primitive.h"
#include "utils/utils.h"
#include "backend/optimizer/common/helper.h"
#include "runtime/device/gpu/kernel_info_setter.h"

namespace luojianet_ms {
namespace opt {
const BaseRef BatchNormReluFusion::DefinePattern() const {
  VectorRef batch_norm = VectorRef({prim::kPrimBatchNorm, x_, scale_, bias_, mean_, var_});
  VectorRef tuple_get = VectorRef({prim::kPrimTupleGetItem, batch_norm, index_});
  VectorRef relu = VectorRef({prim::kPrimRelu, tuple_get});
  return relu;
}

const AnfNodePtr BatchNormReluFusion::Process(const FuncGraphPtr &graph, const AnfNodePtr &node,
                                              const EquivPtr &) const {
  MS_EXCEPTION_IF_NULL(graph);
  MS_EXCEPTION_IF_NULL(node);

  auto tuple_get_item = AnfAlgo::GetInputNode(utils::cast<CNodePtr>(node), 0);
  MS_EXCEPTION_IF_NULL(tuple_get_item);

  // Only fuse output[0] of BatchNorm with ReLU
  size_t output_index = AnfAlgo::GetTupleGetItemOutIndex(utils::cast<CNodePtr>(tuple_get_item));
  if (output_index != 0) {
    return nullptr;
  }

  auto outlist = GetRealNodeUsedList(graph, tuple_get_item);
  // If output[0] of BatchNorm is used by more than one CNode, fusing BatchNorm+ReLU will affect the result of
  // BatchNorm's user node.
  const size_t node_user_num = 1;
  if (outlist->size() != node_user_num) {
    return nullptr;
  }

  auto batch_norm = AnfAlgo::GetInputNode(utils::cast<CNodePtr>(tuple_get_item), 0);
  MS_EXCEPTION_IF_NULL(batch_norm);
  auto is_train = AnfAlgo::GetCNodePrimitive(batch_norm)->GetAttr("is_training");
  MS_EXCEPTION_IF_NULL(is_train);
  if (!GetValue<bool>(is_train)) {
    return nullptr;
  }
  auto format_attr = AnfAlgo::GetCNodePrimitive(batch_norm)->GetAttr("format");
  MS_EXCEPTION_IF_NULL(format_attr);
  auto format = GetValue<std::string>(format_attr);
  if (AnfAlgo::GetInputFormat(batch_norm, 0) != kOpFormat_NHWC && format != "NHWC") {
    return nullptr;
  }
  auto shape = AnfAlgo::GetInputDeviceShape(batch_norm, 0);
  if (shape.back() % kBNChannelMultipleFactor != 0) {
    return nullptr;
  }

  auto x = AnfAlgo::GetInputNode(utils::cast<CNodePtr>(batch_norm), 0);
  auto scale = AnfAlgo::GetInputNode(utils::cast<CNodePtr>(batch_norm), 1);
  auto bias = AnfAlgo::GetInputNode(utils::cast<CNodePtr>(batch_norm), 2);
  auto mean = AnfAlgo::GetInputNode(utils::cast<CNodePtr>(batch_norm), 3);
  auto var = AnfAlgo::GetInputNode(utils::cast<CNodePtr>(batch_norm), 4);

  MS_EXCEPTION_IF_NULL(x);
  MS_EXCEPTION_IF_NULL(scale);
  MS_EXCEPTION_IF_NULL(bias);
  MS_EXCEPTION_IF_NULL(mean);
  MS_EXCEPTION_IF_NULL(var);

  auto prim = std::make_shared<Primitive>(kBatchNormWithActivation);
  MS_EXCEPTION_IF_NULL(prim);
  std::vector<AnfNodePtr> inputs = {NewValueNode(prim), x, scale, bias, mean, var};
  auto fused_batch_norm_with_relu = graph->NewCNode(inputs);
  MS_EXCEPTION_IF_NULL(fused_batch_norm_with_relu);

  std::vector<TypeId> outputs_type;
  std::vector<std::vector<size_t>> outputs_shape;
  auto output_num = AnfAlgo::GetOutputTensorNum(batch_norm);
  for (size_t i = 0; i < output_num; i++) {
    outputs_type.push_back(AnfAlgo::GetOutputInferDataType(batch_norm, i));
    outputs_shape.push_back(AnfAlgo::GetOutputInferShape(batch_norm, i));
  }
  AnfAlgo::SetOutputInferTypeAndShape(outputs_type, outputs_shape, fused_batch_norm_with_relu.get());
  AnfAlgo::CopyNodeAttrs(batch_norm, fused_batch_norm_with_relu);

  auto manager = graph->manager();
  MS_EXCEPTION_IF_NULL(manager);
  manager->Replace(batch_norm, fused_batch_norm_with_relu);
  device::gpu::SetKernelInfo(fused_batch_norm_with_relu);
  return tuple_get_item;
}
}  // namespace opt
}  // namespace luojianet_ms
