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

#include "checker/transpose_checker.h"
#include <vector>
#include <string>
#include "common/data_transpose_utils.h"
#include "common/fetch_content.h"
#include "common/op_attr.h"
#include "common/op_enum.h"

namespace luojianet_ms {
namespace dpico {
bool TransposeChecker::Check(CNodePtr op, int32_t output_num, luojianet_ms::Format format) {
  if (!CheckInputW(op, kInputIndex1, format, kMaxInputWOf4Dims)) {
    MS_LOG(WARNING) << "input_w is not supported. " << op->fullname_with_scope();
    return false;
  }
  auto primitive = GetValueNode<PrimitivePtr>(op->input(0));
  if (primitive == nullptr) {
    MS_LOG(ERROR) << "primitive is nullptr";
    return false;
  }
  DataInfo data_info;
  std::vector<int32_t> perm_val;
  if (op->inputs().size() > kInputIndex2 && FetchDataFromParameterNode(op, kInputIndex2, &data_info) == lite::RET_OK) {
    auto data = reinterpret_cast<int32_t *>(data_info.data_.data());
    if (data == nullptr) {
      MS_LOG(ERROR) << "data is nullptr. " << op->fullname_with_scope();
      return false;
    }
    int data_size;
    if (GetDataSizeFromTensor(&data_info, &data_size) != RET_OK) {
      MS_LOG(ERROR) << "get data size from tensor failed.";
      return false;
    }
    if (static_cast<size_t>(data_size) != kDims4) {
      return false;
    }
    perm_val = {data[0], data[kAxis1], data[kAxis2], data[kAxis3]};
  } else if (primitive->GetAttr(kPerm) != nullptr) {
    perm_val = GetValue<std::vector<int32_t>>(primitive->GetAttr(kPerm));
  } else {
    MS_LOG(ERROR) << "transpose param invalid" << op->fullname_with_scope();
    return false;
  }
  if (perm_val.empty()) {
    MS_LOG(WARNING) << "perm val is empty." << op->fullname_with_scope();
    return false;
  }
  if (perm_val != kNC2NH && perm_val != kNH2NC) {
    MS_LOG(WARNING) << "perm val is unsupported. " << op->fullname_with_scope();
    return false;
  }
  return true;
}

OpCheckerRegistrar g_TransposeChecker("Transpose", new TransposeChecker());  // Permute
}  // namespace dpico
}  // namespace luojianet_ms
