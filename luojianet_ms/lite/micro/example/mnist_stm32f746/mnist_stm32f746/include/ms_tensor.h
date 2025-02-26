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

#ifndef LUOJIANET_MS_LITE_INCLUDE_MS_TENSOR_H_
#define LUOJIANET_MS_LITE_INCLUDE_MS_TENSOR_H_

#include "include/lite_utils.h"
#include "ir/dtype/type_id.h"

namespace luojianet_ms {
enum Format : int64_t ;
namespace tensor {
/// \brief MSTensor defined tensor in LuoJiaNet Lite.
class MS_API MSTensor {
 public:
  /// \brief Constructor of LuoJiaNet Lite MSTensor.
  ///
  /// \return Instance of LuoJiaNet Lite MSTensor.
  MSTensor() = default;

  /// \brief Destructor of LuoJiaNet Lite Model.
  virtual ~MSTensor() = default;

  /// \brief Create a MSTensor.
  ///
  /// \return Pointer to an instance of LuoJiaNet Lite MSTensor.
  static MSTensor *CreateTensor(const String &name, TypeId type, const Vector<int> &shape, const void *data,
                                size_t data_len);

  /// \brief Get memory allocator of current MSTensor.
  ///
  /// \return Pointer of memory allocator class.
  virtual AllocatorPtr allocator() const = 0;

  /// \brief Get data type of the LuoJiaNet Lite MSTensor.
  ///
  /// \note TypeId is defined in luojianet_ms/luojianet_ms/include/api/type_id.h. Only number types in TypeId enum are
  /// suitable for MSTensor.
  ///
  /// \return LuoJiaNet Lite TypeId of the LuoJiaNet Lite MSTensor.
  virtual TypeId data_type() const = 0;

  /// \brief Set data type of current MSTensor.
  ///
  /// \param[in] data_type Define data type, which is shown in type_id.h.
  virtual void set_data_type(TypeId data_type) = 0;

  /// \brief Set format of current MSTensor.
  ///
  /// \param[in] format Define format of data, which is shown in format.h
  virtual void set_format(luojianet_ms::Format format) = 0;

  /// \brief Get format of current MSTensor.
  ///
  /// \return format, which is shown in format.h
  virtual luojianet_ms::Format format() const = 0;

  /// \brief Get shape of the LuoJiaNet Lite MSTensor.
  ///
  /// \return A vector of int as the shape of the LuoJiaNet Lite MSTensor.
  virtual Vector<int> shape() const = 0;

  /// \brief Set the shape of MSTensor.
  virtual void set_shape(const Vector<int> &shape) = 0;

  /// \brief Get number of element in MSTensor.
  ///
  /// \return Number of element in MSTensor.
  virtual int ElementsNum() const = 0;

  /// \brief Get byte size of data in MSTensor.
  ///
  /// \return Byte size of data in MSTensor.
  virtual size_t Size() const = 0;

  /// \brief Get the name of MSTensor.
  ///
  /// \return the name of MSTensor.
  virtual String tensor_name() const = 0;

  /// \brief Set the name of MSTensor.
  virtual void set_tensor_name(const String &name) = 0;

  /// \brief Get the pointer of data in MSTensor.
  ///
  /// \note The data pointer can be used to both write and read data in MSTensor. The memory buffer will be
  /// automatically allocated.
  ///
  /// \return the pointer points to data in MSTensor.
  virtual void *MutableData() = 0;

  /// \brief Get the pointer of data in MSTensor.
  ///
  /// \note The data pointer can be used to both write and read data in MSTensor. No memory buffer will be
  /// allocated.
  ///
  /// \return the pointer points to data in MSTensor.
  virtual void *data() = 0;

  /// \brief Set the data of MSTensor.
  virtual void set_data(void *data) = 0;
};
}  // namespace tensor
}  // namespace luojianet_ms
#endif  // LUOJIANET_MS_LITE_INCLUDE_MS_TENSOR_H_
