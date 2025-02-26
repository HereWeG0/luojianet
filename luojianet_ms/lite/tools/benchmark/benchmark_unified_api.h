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

#ifndef LUOJIANET_MS_BENCHMARK_BENCHMARK_UNIFIED_API_H_
#define LUOJIANET_MS_BENCHMARK_BENCHMARK_UNIFIED_API_H_

#include <signal.h>
#include <random>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <map>
#include <cmath>
#include <string>
#include <vector>
#include <memory>
#include <cfloat>
#include <utility>
#ifndef BENCHMARK_CLIP_JSON
#include <nlohmann/json.hpp>
#endif
#include "tools/benchmark/benchmark_base.h"
#include "include/model.h"
#include "tools/common/flag_parser.h"
#include "src/common/file_utils.h"
#include "src/common/utils.h"
#include "include/api/types.h"
#include "include/api/model.h"
#ifdef ENABLE_OPENGL_TEXTURE
#include "tools/common/opengl_util.h"
#endif

namespace luojianet_ms::lite {
class MS_API BenchmarkUnifiedApi : public BenchmarkBase {
 public:
  explicit BenchmarkUnifiedApi(BenchmarkFlags *flags) : BenchmarkBase(flags) {}

  virtual ~BenchmarkUnifiedApi();

  int RunBenchmark() override;

 protected:
  int CompareDataGetTotalBiasAndSize(const std::string &name, luojianet_ms::MSTensor *tensor, float *total_bias,
                                     int *total_size);
  int CompareDataGetTotalCosineDistanceAndSize(const std::string &name, luojianet_ms::MSTensor *tensor,
                                               float *total_cosine_distance, int *total_size);
  void InitContext(const std::shared_ptr<luojianet_ms::Context> &context);

#ifdef ENABLE_OPENGL_TEXTURE
  int GenerateGLTexture(std::map<std::string, GLuint> *inputGlTexture);

  int LoadAndBindGLTexture();

  int ReadGLTextureFile(std::map<std::string, GLuint> *inputGlTexture);

  int FillGLTextureToTensor(std::map<std::string, GLuint> *gl_texture, luojianet_ms::MSTensor *tensor, std::string name,
                            void *data = nullptr);
#endif

  // call GenerateRandomData to fill inputTensors
  int LoadInput() override;
  int GenerateInputData() override;

  int ReadInputFile() override;

  int InitMSContext(const std::shared_ptr<Context> &context);

  int GetDataTypeByTensorName(const std::string &tensor_name) override;

  int CompareOutput() override;

  int CompareOutputByCosineDistance(float cosine_distance_threshold);

  int InitTimeProfilingCallbackParameter() override;

  int InitPerfProfilingCallbackParameter() override;

  int InitDumpTensorDataCallbackParameter() override;

  int InitPrintTensorDataCallbackParameter() override;

  int PrintInputData();

  template <typename T>
  std::vector<int64_t> ConverterToInt64Vector(const std::vector<T> &srcDims) {
    std::vector<int64_t> dims;
    for (auto shape : srcDims) {
      dims.push_back(static_cast<int64_t>(shape));
    }
    return dims;
  }

  int MarkPerformance();

  int MarkAccuracy();

  void UpdateDistributionName(const std::shared_ptr<luojianet_ms::Context> &context, std::string *name);

 private:
  void UpdateConfigInfo();

 private:
#ifdef ENABLE_OPENGL_TEXTURE
  luojianet_ms::OpenGL::OpenGLRuntime gl_runtime_;
#endif
  luojianet_ms::Model ms_model_;
  std::vector<luojianet_ms::MSTensor> ms_inputs_for_api_;
  std::vector<luojianet_ms::MSTensor> ms_outputs_for_api_;

  MSKernelCallBack ms_before_call_back_ = nullptr;
  MSKernelCallBack ms_after_call_back_ = nullptr;
};

}  // namespace luojianet_ms::lite
#endif  // MINNIE_BENCHMARK_BENCHMARK_H_
