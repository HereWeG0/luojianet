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
#include "common.h"
#include <algorithm>
#include <fstream>
#include <string>
#include <vector>
#include "minddata/dataset/core/client.h"
#include "minddata/dataset/core/config_manager.h"
#include "minddata/dataset/core/pybind_support.h"
#include "minddata/dataset/core/tensor.h"
#include "minddata/dataset/core/tensor_shape.h"
#include "minddata/dataset/engine/datasetops/batch_op.h"
#include "minddata/dataset/engine/datasetops/repeat_op.h"
#include "minddata/dataset/engine/datasetops/source/tf_reader_op.h"

namespace UT {
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

void DatasetOpTesting::SetUp() {
  std::string install_home = "data/dataset";
  datasets_root_path_ = install_home;
  mindrecord_root_path_ = "data/mindrecord";
}

std::vector<luojianet_ms::dataset::TensorShape> DatasetOpTesting::ToTensorShapeVec(
  const std::vector<std::vector<int64_t>> &v) {
  std::vector<luojianet_ms::dataset::TensorShape> ret_v;
  std::transform(v.begin(), v.end(), std::back_inserter(ret_v),
                 [](const auto &s) { return luojianet_ms::dataset::TensorShape(s); });
  return ret_v;
}

std::vector<luojianet_ms::dataset::DataType> DatasetOpTesting::ToDETypes(const std::vector<luojianet_ms::DataType> &t) {
  std::vector<luojianet_ms::dataset::DataType> ret_t;
  std::transform(t.begin(), t.end(), std::back_inserter(ret_t), [](const luojianet_ms::DataType &t) {
    return luojianet_ms::dataset::MSTypeToDEType(static_cast<luojianet_ms::TypeId>(t));
  });
  return ret_t;
}

// Function to read a file into an MSTensor
// Note: This provides the analogous support for DETensor's CreateFromFile.
luojianet_ms::MSTensor DatasetOpTesting::ReadFileToTensor(const std::string &file) {
  if (file.empty()) {
    MS_LOG(ERROR) << "Pointer file is nullptr; return an empty Tensor.";
    return luojianet_ms::MSTensor();
  }
  std::ifstream ifs(file);
  if (!ifs.good()) {
    MS_LOG(ERROR) << "File: " << file << " does not exist; return an empty Tensor.";
    return luojianet_ms::MSTensor();
  }
  if (!ifs.is_open()) {
    MS_LOG(ERROR) << "File: " << file << " open failed; return an empty Tensor.";
    return luojianet_ms::MSTensor();
  }

  ifs.seekg(0, std::ios::end);
  size_t size = ifs.tellg();
  luojianet_ms::MSTensor buf("file", luojianet_ms::DataType::kNumberTypeUInt8, {static_cast<int64_t>(size)}, nullptr, size);

  ifs.seekg(0, std::ios::beg);
  ifs.read(reinterpret_cast<char *>(buf.MutableData()), size);
  ifs.close();

  return buf;
}

// Helper function to create a batch op
std::shared_ptr<luojianet_ms::dataset::BatchOp> DatasetOpTesting::Batch(int32_t batch_size, bool drop,
                                                                     luojianet_ms::dataset::PadInfo pad_map) {
  /*
  std::shared_ptr<luojianet_ms::dataset::ConfigManager> cfg = luojianet_ms::dataset::GlobalContext::config_manager();
  int32_t num_workers = cfg->num_parallel_workers();
  int32_t op_connector_size = cfg->op_connector_size();
  std::vector<std::string> output_columns = {};
  std::vector<std::string> input_columns = {};
  luojianet_ms::dataset::py::function batch_size_func;
  luojianet_ms::dataset::py::function batch_map_func;
  bool pad = false;
  if (!pad_map.empty()) {
    pad = true;
  }
  std::shared_ptr<luojianet_ms::dataset::BatchOp> op =
    std::make_shared<luojianet_ms::dataset::BatchOp>(batch_size, drop, pad, op_connector_size, num_workers, input_columns,
  output_columns, batch_size_func, batch_map_func, pad_map); return op;
  */
  Status rc;
  std::shared_ptr<luojianet_ms::dataset::BatchOp> op;
  rc = luojianet_ms::dataset::BatchOp::Builder(batch_size).SetDrop(drop).SetPaddingMap(pad_map).Build(&op);
  EXPECT_TRUE(rc.IsOk());
  return std::move(op);
}

std::shared_ptr<luojianet_ms::dataset::RepeatOp> DatasetOpTesting::Repeat(int repeat_cnt) {
  std::shared_ptr<luojianet_ms::dataset::RepeatOp> op = std::make_shared<luojianet_ms::dataset::RepeatOp>(repeat_cnt);
  return std::move(op);
}

std::shared_ptr<luojianet_ms::dataset::TFReaderOp> DatasetOpTesting::TFReader(std::string file, int num_works) {
  std::shared_ptr<luojianet_ms::dataset::ConfigManager> config_manager =
    luojianet_ms::dataset::GlobalContext::config_manager();
  auto op_connector_size = config_manager->op_connector_size();
  auto worker_connector_size = config_manager->worker_connector_size();
  std::vector<std::string> columns_to_load = {};
  std::vector<std::string> files = {file};
  std::shared_ptr<luojianet_ms::dataset::TFReaderOp> so = std::make_shared<luojianet_ms::dataset::TFReaderOp>(
    num_works, worker_connector_size, 0, files, std::make_unique<luojianet_ms::dataset::DataSchema>(), op_connector_size,
    columns_to_load, false, 1, 0, false);
  (void)so->Init();
  return std::move(so);
}

std::shared_ptr<luojianet_ms::dataset::ExecutionTree> DatasetOpTesting::Build(
  std::vector<std::shared_ptr<luojianet_ms::dataset::DatasetOp>> ops) {
  std::shared_ptr<luojianet_ms::dataset::ExecutionTree> tree = std::make_shared<luojianet_ms::dataset::ExecutionTree>();
  for (int i = 0; i < ops.size(); i++) {
    tree->AssociateNode(ops[i]);
    if (i > 0) {
      ops[i]->AddChild(std::move(ops[i - 1]));
    }
    if (i == ops.size() - 1) {
      tree->AssignRoot(ops[i]);
    }
  }
  return std::move(tree);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
}  // namespace UT
