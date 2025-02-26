/**
 * Copyright 2019-2021 Huawei Technologies Co., Ltd
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
#ifndef TESTS_UT_CPP_DATASET_COMMON_COMMON_H_
#define TESTS_UT_CPP_DATASET_COMMON_COMMON_H_

#include "gtest/gtest.h"
#include "include/api/status.h"
#include "include/api/types.h"
#include "minddata/dataset/core/client.h"
#include "minddata/dataset/core/tensor_shape.h"
#include "minddata/dataset/core/de_tensor.h"
#include "minddata/dataset/core/type_id.h"
#include "utils/log_adapter.h"
#include "minddata/dataset/engine/datasetops/batch_op.h"
#include "minddata/dataset/engine/datasetops/repeat_op.h"
#include "minddata/dataset/engine/datasetops/source/tf_reader_op.h"

using luojianet_ms::Status;
using luojianet_ms::StatusCode;

#define ASSERT_OK(_s)                          \
  do {                                         \
    Status __rc = (_s);                        \
    if (__rc.IsError()) {                      \
      MS_LOG(ERROR) << __rc.ToString() << "."; \
      ASSERT_TRUE(false);                      \
    }                                          \
  } while (false)

#define EXPECT_OK(_s)                          \
  do {                                         \
    Status __rc = (_s);                        \
    if (__rc.IsError()) {                      \
      MS_LOG(ERROR) << __rc.ToString() << "."; \
      EXPECT_TRUE(false);                      \
    }                                          \
  } while (false)

#define ASSERT_ERROR(_s)                       \
  do {                                         \
    Status __rc = (_s);                        \
    if (__rc.IsOk()) {                         \
      MS_LOG(ERROR) << __rc.ToString() << "."; \
      ASSERT_TRUE(false);                      \
    }                                          \
  } while (false)

#define EXPECT_ERROR(_s)                       \
  do {                                         \
    Status __rc = (_s);                        \
    if (__rc.IsOk()) {                         \
      MS_LOG(ERROR) << __rc.ToString() << "."; \
      EXPECT_TRUE(false);                      \
    }                                          \
  } while (false)

// Macro to compare 2 MSTensors; compare shape-size, shape and data
#define EXPECT_MSTENSOR_EQ(_mstensor1, _mstensor2)                                                      \
  do {                                                                                                  \
    EXPECT_EQ(_mstensor1.Shape().size(), _mstensor2.Shape().size());                                    \
    for (int i = 0; i < _mstensor1.Shape().size(); i++) {                                               \
      EXPECT_EQ(_mstensor1.Shape()[i], _mstensor2.Shape()[i]);                                          \
    }                                                                                                   \
    EXPECT_EQ(_mstensor1.DataSize(), _mstensor2.DataSize());                                            \
    EXPECT_EQ(std::memcmp((const void *)_mstensor1.Data().get(), (const void *)_mstensor2.Data().get(), \
                          _mstensor2.DataSize()),                                                       \
              0);                                                                                       \
  } while (false)

// Macro to invoke MS_LOG for MSTensor
#define TEST_MS_LOG_MSTENSOR(_loglevel, _msg, _mstensor)           \
  do {                                                             \
    std::shared_ptr<Tensor> _de_tensor;                            \
    ASSERT_OK(Tensor::CreateFromMSTensor(_mstensor, &_de_tensor)); \
    MS_LOG(_loglevel) << _msg << *_de_tensor;                      \
  } while (false)

namespace UT {
class Common : public testing::Test {
 public:
  // every TEST_F macro will enter one
  virtual void SetUp();

  virtual void TearDown();
};

class DatasetOpTesting : public Common {
 public:
  // Helper functions for creating datasets
  std::shared_ptr<luojianet_ms::dataset::BatchOp> Batch(int32_t batch_size = 1, bool drop = false,
                                                     luojianet_ms::dataset::PadInfo = {});

  std::shared_ptr<luojianet_ms::dataset::RepeatOp> Repeat(int repeat_cnt = 1);

  std::shared_ptr<luojianet_ms::dataset::TFReaderOp> TFReader(std::string file, int num_works = 8);

  std::shared_ptr<luojianet_ms::dataset::ExecutionTree> Build(
    std::vector<std::shared_ptr<luojianet_ms::dataset::DatasetOp>> ops);

  std::vector<luojianet_ms::dataset::TensorShape> ToTensorShapeVec(const std::vector<std::vector<int64_t>> &v);
  std::vector<luojianet_ms::dataset::DataType> ToDETypes(const std::vector<luojianet_ms::DataType> &t);
  luojianet_ms::MSTensor ReadFileToTensor(const std::string &file);
  std::string datasets_root_path_;
  std::string mindrecord_root_path_;
  void SetUp() override;
};
}  // namespace UT
#endif  // TESTS_UT_CPP_DATASET_COMMON_COMMON_H_
