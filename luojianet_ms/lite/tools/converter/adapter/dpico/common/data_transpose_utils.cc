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
#include "common/data_transpose_utils.h"
#include <memory>
#include <string>
#include <utility>
#include <unordered_map>
#include <vector>
#include "common/check_base.h"

namespace luojianet_ms {
namespace dpico {
namespace {
const std::unordered_map<luojianet_ms::Format, std::string> kTensorFormatMap{
  {luojianet_ms::Format::NCHW, "NCHW"}, {luojianet_ms::Format::NHWC, "NHWC"},     {luojianet_ms::Format::NHWC4, "NHWC4"},
  {luojianet_ms::Format::HWKC, "HWKC"}, {luojianet_ms::Format::HWCK, "HWCK"},     {luojianet_ms::Format::KCHW, "KCHW"},
  {luojianet_ms::Format::CKHW, "CKHW"}, {luojianet_ms::Format::KHWC, "KHWC"},     {luojianet_ms::Format::CHWK, "CHWK"},
  {luojianet_ms::Format::HW, "HW"},     {luojianet_ms::Format::HW4, "HW4"},       {luojianet_ms::Format::NC, "NC"},
  {luojianet_ms::Format::NC4, "NC4"},   {luojianet_ms::Format::NC4HW4, "NC4HW4"}, {luojianet_ms::Format::NCDHW, "NCDHW"},
  {luojianet_ms::Format::NCW, "NCW"},   {luojianet_ms::Format::NWC, "NWC"}};
int GetPre(int idx, int row, int col) {  // computer before transpose the position that current element was
  if (row == 0 || col == 0) {
    return -1;
  }
  return (idx % row) * col + idx / row;
}
void MoveData(float *matrix, int idx, int row, int col) {  // idx is from the new matrix
  float tmp = matrix[idx];
  int cur = idx;
  int pre = GetPre(idx, row, col);
  while (pre != idx) {
    matrix[cur] = matrix[pre];
    cur = pre;
    pre = GetPre(cur, row, col);
  }
  matrix[cur] = tmp;
}
}  // namespace

int DeduceDimConvertion(luojianet_ms::Format src_format, luojianet_ms::Format dst_format, std::vector<int> *perm) {
  MS_ASSERT(perm != nullptr);
  if (kTensorFormatMap.find(src_format) == kTensorFormatMap.end() ||
      kTensorFormatMap.find(dst_format) == kTensorFormatMap.end()) {
    MS_LOG(ERROR) << "src_format or dst_format is error.";
    return RET_ERROR;
  }
  auto src_format_str = kTensorFormatMap.at(src_format);
  auto dst_format_str = kTensorFormatMap.at(dst_format);
  if (src_format_str.size() != dst_format_str.size()) {
    MS_LOG(ERROR) << "src_format or dst_format is error.";
    return RET_ERROR;
  }
  std::replace(src_format_str.begin(), src_format_str.end(), 'K', 'N');
  std::replace(dst_format_str.begin(), dst_format_str.end(), 'K', 'N');
  perm->clear();
  std::unordered_map<char, int> dim_map;
  for (size_t i = 0; i < src_format_str.size(); ++i) {
    dim_map[src_format_str[i]] = i;
  }
  for (size_t i = 0; i < dst_format_str.size(); ++i) {
    if (dim_map.find(dst_format_str[i]) == dim_map.end()) {
      MS_LOG(ERROR) << "src_format and dst_format cannot match, please check.";
      return RET_ERROR;
    }
    perm->push_back(dim_map[dst_format_str[i]]);
  }
  return RET_OK;
}

template <typename T>
STATUS TransposeData(const ShapeVector &origin_shape, const ShapeVector &cur_shape, const std::vector<int> &perm,
                     T *weight_data, std::vector<T> *buf) {
  MS_ASSERT(weight_data != nullptr && buf != nullptr);
  MS_ASSERT(origin_shape.size() == cur_shape.size() && cur_shape.size() == perm.size());
  int count = 1;
  for (size_t i = 0; i < origin_shape.size(); i++) {
    if (INT_MUL_OVERFLOW(count, origin_shape.at(i))) {
      MS_LOG(ERROR) << "int mul overflow.";
      return RET_ERROR;
    }
    count *= origin_shape.at(i);
  }
  ShapeVector post_multiply(cur_shape.size());
  std::unordered_map<int, int> dim_map;
  for (int i = cur_shape.size() - 1; i >= 0; --i) {
    if (i == static_cast<int>(cur_shape.size() - 1)) {
      post_multiply[i] = 1;
    } else {
      post_multiply[i] = cur_shape[i + 1] * post_multiply[i + 1];
    }
    dim_map[perm[i]] = i;
  }
  std::unordered_map<int, int> position_map;
  for (int i = 0; i < count; ++i) {
    int temp = i;
    for (int j = static_cast<int>(origin_shape.size()) - 1; j >= 0; --j) {
      MS_ASSERT(origin_shape[j] > 0);
      position_map[j] = temp % origin_shape[j];
      temp /= origin_shape[j];
    }
    int64_t new_pos = std::accumulate(position_map.begin(), position_map.end(), 0,
                                      [&post_multiply, &dim_map](int64_t res, const std::pair<int, int> &pair_y) {
                                        return res + post_multiply[dim_map[pair_y.first]] * pair_y.second;
                                      });
    buf->at(new_pos) = weight_data[i];
  }
  return RET_OK;
}

template <typename T>
STATUS DoTransposeData(const tensor::TensorPtr &tensor, luojianet_ms::Format src_format, luojianet_ms::Format dst_format) {
  MS_ASSERT(tensor != nullptr);
  auto origin_shape = tensor->shape_c();
  if (origin_shape.size() != kDims4) {
    MS_LOG(ERROR) << "Filter dim-num is not supported, dim-num: " << origin_shape.size();
    return lite::RET_ERROR;
  }
  if (std::any_of(origin_shape.begin(), origin_shape.end(), [](int64_t val) { return val <= 0; })) {
    MS_LOG(ERROR) << "the tensor's shape is invalid.";
    return lite::RET_ERROR;
  }
  std::vector<int> perm;
  if (DeduceDimConvertion(src_format, dst_format, &perm) != RET_OK) {
    MS_LOG(ERROR) << "deduce perm failed.";
    return lite::RET_ERROR;
  }
  ShapeVector new_shape;
  for (auto &val : perm) {
    if (val < 0 || static_cast<size_t>(val) >= origin_shape.size()) {
      MS_LOG(ERROR) << "deduce perm is invalid.";
      return lite::RET_ERROR;
    }
    new_shape.push_back(origin_shape[val]);
  }
  int64_t count = 1;
  for (size_t i = 0; i < origin_shape.size(); i++) {
    if (INT_MUL_OVERFLOW(count, origin_shape.at(i))) {
      MS_LOG(ERROR) << "int mul overflow.";
      return RET_ERROR;
    }
    count *= origin_shape.at(i);
  }
  if (count <= 0 || count > static_cast<int64_t>(INT32_MAX)) {
    MS_LOG(ERROR) << "tensor element num is too big, which should be smaller than int32_max.";
    return RET_ERROR;
  }
  std::vector<T> buf(count);

  auto origin_weight_data = tensor->data_c();
  if (origin_weight_data == nullptr) {
    MS_LOG(ERROR) << "origin_weight_data is nullptr.";
    return RET_ERROR;
  }
  T *weightData = static_cast<T *>(origin_weight_data);
  if (TransposeData<T>(origin_shape, new_shape, perm, weightData, &buf) != RET_OK) {
    MS_LOG(ERROR) << "transpose data failed.";
    return RET_ERROR;
  }
  if (tensor->Size() == 0) {
    MS_LOG(ERROR) << "tensor size shouldn't be 0";
    return RET_ERROR;
  }
  if (memcpy_s(tensor->data_c(), tensor->Size(), buf.data(), count * sizeof(T)) != EOK) {
    MS_LOG(ERROR) << "memcpy_s failed.";
    return RET_ERROR;
  }
  tensor->set_shape(new_shape);
  return RET_OK;
}

STATUS TransFilterFormat(const tensor::TensorPtr &tensor, luojianet_ms::Format src_format, luojianet_ms::Format dst_format) {
  if (tensor == nullptr) {
    MS_LOG(ERROR) << "tensor is nullptr.";
    return RET_ERROR;
  }
  std::unordered_map<TypeId, std::function<STATUS(const tensor::TensorPtr &, luojianet_ms::Format, luojianet_ms::Format)>>
    trans_func = {{kNumberTypeFloat32, DoTransposeData<float>},
                  {kNumberTypeUInt8, DoTransposeData<uint8_t>},
                  {kNumberTypeInt8, DoTransposeData<int8_t>},
                  {kNumberTypeFloat16, DoTransposeData<float16>}};
  auto data_type = tensor->data_type();
  auto iter = trans_func.find(data_type);
  if (iter == trans_func.end()) {
    MS_LOG(ERROR) << "Unsupported data_type: " << data_type;
    return RET_ERROR;
  }
  return iter->second(tensor, src_format, dst_format);
}
int GetNext(int idx, int row, int col) {  // compute after transpose the position that current element will move to
  if (col == 0) {
    return -1;
  }
  return (idx % col) * row +
         idx / col;  // (idx / col, idx % col) -> (idx % col, idx / col) -> (idx % col) * row + idx / col
}
void TransposeMatrix(float *matrix, int row, int col) {
  for (int i = 0; i < row * col; i++) {
    int next = GetNext(i, row, col);
    while (next > i) {
      next = GetNext(next, row, col);
    }
    if (next == i) {  // otherwise, current element has been moved.
      MoveData(matrix, i, row, col);
    }
  }
}
}  // namespace dpico
}  // namespace luojianet_ms
