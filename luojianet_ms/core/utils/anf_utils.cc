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

#include "utils/anf_utils.h"
#include <string>
#include "base/core_ops.h"
#include "utils/trace_base.h"
#include "utils/utils.h"

namespace luojianet_ms {
namespace {
const PrimitiveSet follow_first_input_prims = {prim::kPrimDepend, prim::kPrimLoad};
}  // namespace

bool AnfUtils::IsDimUnknown(const abstract::ShapePtr &shape) {
  MS_EXCEPTION_IF_NULL(shape);
  return std::any_of(shape->shape().begin(), shape->shape().end(), [](int64_t s) { return s < -1; });
}

bool AnfUtils::IsShapeDynamic(const abstract::ShapePtr &shape) {
  MS_EXCEPTION_IF_NULL(shape);
  return std::any_of(shape->shape().begin(), shape->shape().end(), [](int64_t s) { return s < 0; });
}

bool AnfUtils::IsShapeDynamic(const std::vector<size_t> &shape) {
  return std::any_of(shape.begin(), shape.end(), [](int64_t s) { return s < 0; });
}

bool AnfUtils::IsNodeOutputDynamicShape(const CNodePtr &node) {
  MS_EXCEPTION_IF_NULL(node);
  auto base_shape = node->Shape();
  if (base_shape == nullptr) {
    MS_LOG(INFO) << "Invalid base shape, node: " << node->fullname_with_scope();
    return false;
  }
  if (base_shape->isa<abstract::Shape>() && IsShapeDynamic(base_shape->cast<abstract::ShapePtr>())) {
    return true;
  } else if (base_shape->isa<abstract::TupleShape>()) {
    auto tuple_shape = base_shape->cast<abstract::TupleShapePtr>();
    MS_EXCEPTION_IF_NULL(tuple_shape);
    for (size_t i = 0; i < tuple_shape->size(); i++) {
      auto b_shape = (*tuple_shape)[i];
      if (b_shape->isa<abstract::Shape>() && IsShapeDynamic(b_shape->cast<abstract::ShapePtr>())) {
        return true;
      }
    }
  }
  return false;
}

bool AnfUtils::IsDimUnknown(const AnfNodePtr &node) {
  MS_EXCEPTION_IF_NULL(node);
  auto base_shape = node->Shape();
  if (base_shape == nullptr) {
    MS_LOG(INFO) << "Invalid base shape, node: " << node->fullname_with_scope();
    return false;
  }
  if (base_shape->isa<abstract::Shape>()) {
    auto base_shape_ptr = base_shape->cast<abstract::ShapePtr>();
    MS_EXCEPTION_IF_NULL(base_shape_ptr);
    return base_shape_ptr->IsDimUnknown();
  } else if (base_shape->isa<abstract::TupleShape>()) {
    auto tuple_shape_ptr = base_shape->cast<abstract::TupleShapePtr>();
    MS_EXCEPTION_IF_NULL(tuple_shape_ptr);
    return tuple_shape_ptr->IsDimUnknown();
  } else if (base_shape->isa<abstract::SequenceShape>()) {
    auto seq_shape_ptr = base_shape->cast<abstract::SequenceShapePtr>();
    MS_EXCEPTION_IF_NULL(seq_shape_ptr);
    return seq_shape_ptr->IsDimUnknown();
  } else if (base_shape->isa<abstract::ListShape>()) {
    auto list_shape_ptr = base_shape->cast<abstract::ListShapePtr>();
    MS_EXCEPTION_IF_NULL(list_shape_ptr);
    return list_shape_ptr->IsDimUnknown();
  }
  return false;
}

bool AnfUtils::IsRealKernel(const AnfNodePtr &node) {
  MS_EXCEPTION_IF_NULL(node);
#ifndef ENABLE_SECURITY
  static const PrimitiveSet virtual_prims = {
    prim::kPrimImageSummary, prim::kPrimScalarSummary, prim::kPrimTensorSummary, prim::kPrimHistogramSummary,
    prim::kPrimMakeTuple,    prim::kPrimStateSetItem,  prim::kPrimTupleGetItem,  prim::kPrimReturn,
    prim::kPrimPartial,      prim::kPrimDepend,        prim::kPrimUpdateState,   prim::kPrimLoad};
#else
  static const PrimitiveSet virtual_prims = {prim::kPrimMakeTuple,   prim::kPrimStateSetItem, prim::kPrimTupleGetItem,
                                             prim::kPrimReturn,      prim::kPrimPartial,      prim::kPrimDepend,
                                             prim::kPrimUpdateState, prim::kPrimLoad};
#endif
  auto cnode = node->cast<CNodePtr>();
  if (cnode == nullptr) {
    // parameter and value node is a real kernel too
    return true;
  }
  if (cnode->size() == 0) {
    MS_LOG(EXCEPTION) << "Illegal null input of cnode(%s)" << node->DebugString() << trace::DumpSourceLines(node);
  }

  auto kernel_info = cnode->kernel_info();
  if (kernel_info) {
    auto runtime_cache = kernel_info->runtime_cache();
    MS_EXCEPTION_IF_NULL(runtime_cache);
    if (runtime_cache->is_real_kernel() != CacheBool::UNCACHED) {
      return (runtime_cache->is_real_kernel() == CacheBool::TRUE);
    }
  }
  bool res = !IsOneOfPrimitive(cnode->input(kAnfPrimitiveIndex), virtual_prims);

  if (kernel_info) {
    auto runtime_cache = kernel_info->runtime_cache();
    MS_EXCEPTION_IF_NULL(runtime_cache);
    if (res) {
      runtime_cache->set_real_kernel(CacheBool::TRUE);
    } else {
      runtime_cache->set_real_kernel(CacheBool::FALSE);
    }
  }

  return res;
}

bool AnfUtils::IsRealCNodeKernel(const AnfNodePtr &node) {
  MS_EXCEPTION_IF_NULL(node);
  if (!node->isa<CNode>()) {
    return false;
  }
  if (IsPrimitiveCNode(node, prim::kPrimReturn)) {
    return true;
  }
  return AnfUtils::IsRealKernel(node);
}

std::string AnfUtils::GetCNodeName(const AnfNodePtr &node) {
  MS_EXCEPTION_IF_NULL(node);
  if (node->isa<CNode>()) {
    auto primitive = GetCNodePrimitive(node);
    if (primitive != nullptr) {
      if (primitive->name() == "Custom") {
        auto uniq_name = primitive->GetAttr("uniq_name");
        if (uniq_name) {
          return GetValue<std::string>(uniq_name);
        }
      }
      return primitive->name();
    }
    auto func_graph = GetCNodeFuncGraph(node);
    MS_EXCEPTION_IF_NULL(func_graph);
    if (func_graph->has_attr(FUNC_GRAPH_ATTR_GRAPH_KERNEL)) {
      std::string fg_name = "GraphKernel_";
      fg_name += GetValue<std::string>(func_graph->get_attr(FUNC_GRAPH_ATTR_GRAPH_KERNEL));
      return fg_name;
    }
    return func_graph->ToString();
  }
  MS_LOG(EXCEPTION) << "Unknown anf node type " << node->DebugString() << trace::DumpSourceLines(node);
}

size_t AnfUtils::GetInputTensorNum(const AnfNodePtr &node) {
  MS_EXCEPTION_IF_NULL(node);
  auto cnode = node->cast<CNodePtr>();
  if (cnode == nullptr) {
    MS_LOG(EXCEPTION) << "Only cnode has real input, but this anf is " << node->DebugString()
                      << trace::DumpSourceLines(node);
  }
  ssize_t input_tensor_num = cnode->input_tensor_num();
  if (input_tensor_num >= 0) {
    return static_cast<size_t>(input_tensor_num);
  }
  size_t input_num = cnode->inputs().size();
  if (input_num == 0) {
    MS_LOG(EXCEPTION) << "Cnode inputs size can't be zero" << trace::DumpSourceLines(node);
  }
  // Exclude inputs[0].
  --input_num;

  // Exclude monad inputs for real cnodes.
  if (input_num > 0 && AnfUtils::IsRealKernel(cnode)) {
    auto &inputs = cnode->inputs();
    // Search monad inputs, backward.
    for (auto iter = inputs.rbegin(); iter != inputs.rend(); ++iter) {
      if (!HasAbstractMonad(*iter)) {
        // Stop count if we encounter a non-monad input.
        break;
      }
      --input_num;
    }
  }
  cnode->set_input_tensor_num(static_cast<ssize_t>(input_num));
  return input_num;
}

size_t AnfUtils::GetOutputTensorNum(const AnfNodePtr &node) {
  MS_EXCEPTION_IF_NULL(node);
  auto kernel_info = node->kernel_info();
  if (kernel_info) {
    auto runtime_cache = kernel_info->runtime_cache();
    if (runtime_cache->is_valid()) {
      ssize_t output_tensor_num = runtime_cache->output_tensor_num();
      if (output_tensor_num >= 0) {
        return static_cast<size_t>(output_tensor_num);
      }
    }
  }

  size_t res;
  TypePtr type = node->Type();
  if (type == nullptr) {
    res = 0;
  } else if (type->isa<Tuple>()) {
    auto tuple_type = type->cast<TuplePtr>();
    MS_EXCEPTION_IF_NULL(tuple_type);
    res = tuple_type->size();
  } else if (type->isa<TypeNone>()) {
    res = 0;
  } else {
    res = 1;
  }

  if (kernel_info) {
    auto runtime_cache = kernel_info->runtime_cache();
    if (runtime_cache->is_valid()) {
      runtime_cache->set_output_tensor_num(static_cast<ssize_t>(res));
    }
  }
  return res;
}

std::pair<AnfNodePtr, size_t> AnfUtils::VisitKernel(const AnfNodePtr &anf_node, size_t index) {
  MS_EXCEPTION_IF_NULL(anf_node);
  if (anf_node->isa<ValueNode>()) {
    return std::make_pair(anf_node, 0);
  } else if (anf_node->isa<Parameter>()) {
    return std::make_pair(anf_node, 0);
  } else if (anf_node->isa<CNode>()) {
    auto cnode = anf_node->cast<CNodePtr>();
    MS_EXCEPTION_IF_NULL(cnode);
    auto input0 = cnode->input(0);
    MS_EXCEPTION_IF_NULL(input0);
    if (IsPrimitive(input0, prim::kPrimMakeTuple)) {
      if (GetInputTensorNum(cnode) == 0) {
        return std::make_pair(nullptr, 0);
      }
      auto node = cnode->input(index + IntToSize(1));
      MS_EXCEPTION_IF_NULL(node);
      return VisitKernel(node, 0);
    } else if (IsPrimitive(input0, prim::kPrimTupleGetItem)) {
      if (cnode->inputs().size() != kTupleGetItemInputSize) {
        MS_LOG(EXCEPTION) << "The node tuple_get_item must have 2 inputs!";
      }
      auto input2 = cnode->input(kInputNodeOutputIndexInTupleGetItem);
      MS_EXCEPTION_IF_NULL(input2);
      auto value_node = input2->cast<ValueNodePtr>();
      MS_EXCEPTION_IF_NULL(value_node);
      auto item_idx = GetValue<int64_t>(value_node->value());
      return VisitKernel(cnode->input(kRealInputNodeIndexInTupleGetItem), LongToSize(item_idx));
    } else if (IsPrimitiveCNode(cnode, prim::kPrimUpdateState)) {
      return VisitKernel(cnode->input(kUpdateStateRealInput), 0);
    } else if (IsOneOfPrimitive(input0, follow_first_input_prims)) {
      return VisitKernel(cnode->input(kRealInputIndexInDepend), 0);
    } else {
      return std::make_pair(anf_node, index);
    }
  } else {
    MS_LOG(EXCEPTION) << "The input is invalid";
  }
}

bool AnfUtils::IsGraphKernel(const AnfNodePtr &node) {
  MS_EXCEPTION_IF_NULL(node);
  auto func_graph = GetCNodeFuncGraph(node);
  return func_graph != nullptr && func_graph->has_attr(FUNC_GRAPH_ATTR_GRAPH_KERNEL);
}

bool AnfUtils::IsNodeInGraphKernel(const AnfNodePtr &node) {
  MS_EXCEPTION_IF_NULL(node);
  return node->func_graph() != nullptr && node->func_graph()->has_attr(FUNC_GRAPH_ATTR_GRAPH_KERNEL);
}

void AnfUtils::SetDumpFlag(const AnfNodePtr &node) {
  if (node == nullptr || !node->isa<CNode>()) {
    return;
  }
  auto prim = GetCNodePrimitive(node);
  if (prim != nullptr) {
    prim->set_attr(kAttrDump, MakeValue(kValueTrue));
  }
}

bool AnfUtils::GetDumpFlag(const AnfNodePtr &node) {
  if (node == nullptr || !node->isa<CNode>()) {
    return false;
  }
  auto prim = GetCNodePrimitive(node);
  if (prim != nullptr) {
    auto attr = prim->GetAttr(kAttrDump);
    if (attr != nullptr && attr->isa<StringImm>() && attr->cast<StringImmPtr>()->value() == kValueTrue) {
      return true;
    }
  }
  return false;
}
}  // namespace luojianet_ms
