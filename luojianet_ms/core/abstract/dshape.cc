/**
 * This is the C++ adaptation and derivative work of Myia (https://github.com/mila-iqia/myia/).
 *
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

#include "abstract/dshape.h"

#include <exception>
#include <iostream>

#include "utils/log_adapter.h"

namespace luojianet_ms {
namespace abstract {
namespace {
std::string ShapeVectorToStr(const std::vector<int64_t> &shp) {
  std::ostringstream buffer;
  bool f_begin = true;
  buffer << "(";
  for (auto &x : shp) {
    if (!f_begin) {
      buffer << ", ";
    } else {
      f_begin = false;
    }
    buffer << x;
  }
  buffer << ")";
  return buffer.str();
}
}  // namespace
// used for print BaseShape content
std::ostream &operator<<(std::ostream &os, const BaseShape &bs) {
  os << bs.ToString();
  return os;
}

std::ostream &operator<<(std::ostream &os, const std::shared_ptr<BaseShape> bs) {
  MS_EXCEPTION_IF_NULL(bs);
  os << bs->ToString();
  return os;
}

bool BaseShape::operator==(const BaseShape &other) const {
  if (tid() != other.tid()) {
    return false;
  }
  return true;
}

bool BaseShape::operator!=(const BaseShape &other) const { return !(*this == other); }

std::string Shape::ToString() const {
  std::ostringstream buffer;
  bool has_dyn_shape = IsDynamic();
  if (has_dyn_shape) {
    buffer << "{shape:";
  }
  buffer << ShapeVectorToStr(shape_);
  if (has_dyn_shape) {
    buffer << "|min shape:";
    buffer << ShapeVectorToStr(min_shape_);
    buffer << "|max shape:";
    buffer << ShapeVectorToStr(max_shape_);
    buffer << "}";
  }
  return buffer.str();
}

std::string Shape::DumpText() const {
  std::ostringstream buffer;
  buffer << "[";
  for (size_t i = 0; i < shape_.size(); i++) {
    buffer << (i > 0 ? ", " : "") << shape_[i];
    if (shape_[i] == SHP_ANY && min_shape_.size() == shape_.size() && max_shape_.size() == shape_.size()) {
      buffer << "_" << min_shape_[i] << "^" << max_shape_[i];
    }
  }
  buffer << "]";
  return buffer.str();
}

bool Shape::operator==(const BaseShape &other) const {
  if (tid() != other.tid()) {
    return false;
  }
  Shape other_shape = static_cast<const Shape &>(other);
  bool shape_equal = shape_ == other_shape.shape_;

  if (!IsDynamic() || !other_shape.IsDynamic()) {
    return shape_equal;
  }
  bool min_shape_equel = min_shape_ == other_shape.min_shape_;
  bool max_shape_equel = max_shape_ == other_shape.max_shape_;
  return shape_equal && min_shape_equel && max_shape_equel;
}

const int64_t Shape::SHP_ANY;
void Shape::Broaden() {
  for (size_t i = 0; i < shape_.size(); i++) {
    shape_[i] = SHP_ANY;
  }
}

std::string SequenceShape::ToString() const {
  std::ostringstream buffer;
  bool f_begin = true;
  for (const auto &p_shp : p_shapes_) {
    if (!f_begin) {
      buffer << ", ";
    } else {
      f_begin = false;
    }
    MS_EXCEPTION_IF_NULL(p_shp);
    buffer << p_shp->ToString();
  }
  return buffer.str();
}

BaseShapePtrList SequenceShape::ElementsClone() const {
  BaseShapePtrList ele_list;
  for (auto p_shp : p_shapes_) {
    MS_EXCEPTION_IF_NULL(p_shp);
    ele_list.push_back(p_shp->Clone());
  }
  return ele_list;
}

template <typename T>
bool SequenceShape::SequenceEqual(const BaseShape &other) const {
  if (tid() != other.tid()) {
    return false;
  }
  auto other_shapes = static_cast<const T &>(other).p_shapes_;
  if (other_shapes.size() != p_shapes_.size()) {
    return false;
  }
  for (uint64_t i = 0; i < p_shapes_.size(); ++i) {
    MS_EXCEPTION_IF_NULL(p_shapes_[i]);
    MS_EXCEPTION_IF_NULL(other_shapes[i]);
    if (!(*p_shapes_[i] == *other_shapes[i])) {
      return false;
    }
  }
  return true;
}
template bool SequenceShape::SequenceEqual<TupleShape>(const BaseShape &) const;
template bool SequenceShape::SequenceEqual<ListShape>(const BaseShape &) const;
}  // namespace abstract
}  // namespace luojianet_ms
