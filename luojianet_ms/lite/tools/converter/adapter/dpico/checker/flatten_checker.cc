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

#include "checker/flatten_checker.h"
#include <vector>
#include <string>
#include "common/op_attr.h"
#include "common/op_enum.h"

namespace luojianet_ms {
namespace dpico {
namespace {
constexpr int kMaxFlattenInputW = 65536;
}
bool FlattenChecker::Check(CNodePtr op, int32_t output_num, luojianet_ms::Format format) {
  auto primitive = GetValueNode<PrimitivePtr>(op->input(0));
  if (primitive == nullptr) {
    MS_LOG(ERROR) << "primitive is nullptr." << op->fullname_with_scope();
    return false;
  }
  if (primitive->GetAttr(kStartAxis) != nullptr) {
    auto axis = GetValue<int32_t>(primitive->GetAttr(kStartAxis));
    if (axis < kAxisLowerBound || axis > kAxisUpperBound) {
      MS_LOG(WARNING) << "start_axis val should in range [-4, 3] " << op->fullname_with_scope();
      return false;
    }
  }
  if (primitive->GetAttr(kEndAxis) != nullptr) {
    auto end_axis = GetValue<int32_t>(primitive->GetAttr(kEndAxis));
    if (end_axis < kAxisLowerBound || end_axis > kAxisUpperBound) {
      MS_LOG(WARNING) << "end_axis val should in range [-4, 3] " << op->fullname_with_scope();
      return false;
    }
  }

  if (!CheckInputW(op, 1, format, kMaxFlattenInputW)) {
    MS_LOG(WARNING) << "input_w is not supported. " << op->fullname_with_scope();
    return false;
  }
  return true;
}
OpCheckerRegistrar g_FlattenChecker("Flatten", new FlattenChecker());
}  // namespace dpico
}  // namespace luojianet_ms
