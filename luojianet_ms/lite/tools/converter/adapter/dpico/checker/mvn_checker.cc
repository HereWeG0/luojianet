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

#include "checker/mvn_checker.h"
#include <vector>
#include <algorithm>
#include <string>
#include "common/anf_util.h"
#include "common/op_enum.h"
#include "common/fetch_content.h"

namespace luojianet_ms {
namespace dpico {
bool MvnChecker::Check(CNodePtr op, int32_t output_num, luojianet_ms::Format format) {
  if (!CheckInputW(op, kInputIndex1, format, kMaxInputWOf4Dims)) {
    MS_LOG(WARNING) << "input_w is not supported. " << op->fullname_with_scope();
    return false;
  }

  auto primitive = GetValueNode<PrimitivePtr>(op->input(0));
  if (primitive == nullptr) {
    MS_LOG(ERROR) << "primitive is nullptr";
    return false;
  }

  if (primitive->GetAttr(ops::kAxes) != nullptr) {
    auto axes = GetValue<std::vector<uint32_t>>(primitive->GetAttr(ops::kAxes));
    if (axes != std::vector<uint32_t>{0, kAxis2, kAxis3} && axes != std::vector<uint32_t>{0, 1, kAxis2, kAxis3}) {
      MS_LOG(WARNING) << "axes only supports [0,2,3] or [0,1,2,3] " << op->fullname_with_scope();
      return false;
    }
  }
  return true;
}

OpCheckerRegistrar g_MvnChecker("Mvn", new MvnChecker());
}  // namespace dpico
}  // namespace luojianet_ms
