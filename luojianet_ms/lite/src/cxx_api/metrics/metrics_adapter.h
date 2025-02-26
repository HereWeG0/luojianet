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

#ifndef LUOJIANET_MS_LITE_SRC_CXX_API_METRICS_METRICS_ADAPTER_H_
#define LUOJIANET_MS_LITE_SRC_CXX_API_METRICS_METRICS_ADAPTER_H_

#include <functional>
#include <map>
#include <string>
#include <vector>
#include <memory>
#include <utility>
#include <unordered_map>
#include "include/api/model.h"
#include "include/api/context.h"
#include "include/api/cell.h"
#include "include/api/metrics/metrics.h"
#include "include/lite_session.h"
#include "include/train/metrics.h"

namespace luojianet_ms {

class MetricsAdapter : public session::Metrics {
 public:
  explicit MetricsAdapter(luojianet_ms::Metrics *metrics) : metrics_(metrics) {}
  MetricsAdapter() = delete;

  void Clear() override { metrics_->Clear(); }

  float Eval() override { return metrics_->Eval(); }
  void Update(std::vector<tensor::MSTensor *> inputs, std::vector<tensor::MSTensor *> outputs) override {
    // metrics_->Update(inputs, outputs); TODO need to implement
  }

 private:
  luojianet_ms::Metrics *metrics_;
};
}  // namespace luojianet_ms

#endif  // LUOJIANET_MS_LITE_SRC_CXX_API_METRICS_METRICS_ADAPTER_H_
