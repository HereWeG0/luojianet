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

#ifndef LUOJIANET_MS_CORE_OPS_LSTM_GRAD_H_
#define LUOJIANET_MS_CORE_OPS_LSTM_GRAD_H_
#include <map>
#include <vector>
#include <string>
#include <memory>
#include "ops/primitive_c.h"
#include "abstract/abstract_value.h"
#include "utils/check_convert_utils.h"

namespace luojianet_ms {
namespace ops {
constexpr auto kNameLSTMGrad = "LSTMGrad";
class MS_CORE_API LSTMGrad : public PrimitiveC {
 public:
  LSTMGrad() : PrimitiveC(kNameLSTMGrad) {}
  ~LSTMGrad() = default;
  MS_DECLARE_PARENT(LSTMGrad, PrimitiveC);
  void Init(const int64_t input_size, const int64_t hidden_size, const int64_t num_layers, const bool has_bias,
            const float dropout, const bool bidirectional = false, const float zoneout_cell = 0.0f,
            const float zoneout_hidden = 0.0f);
  void set_input_size(const int64_t input_size);
  int64_t get_input_size() const;
  void set_hidden_size(const int64_t hidden_size);
  int64_t get_hidden_size() const;
  void set_num_layers(const int64_t num_layers);
  int64_t get_num_layers() const;
  void set_has_bias(const bool has_bias);
  bool get_has_bias() const;
  void set_dropout(const float dropout);
  float get_dropout() const;
  void set_bidirectional(const bool bidirectional);
  bool get_bidirectional() const;
  void set_num_directions(const int64_t num_directions);
  int64_t get_num_directions() const;
  void set_zoneout_cell(float zoneout_cell);
  float get_zoneout_cell() const;
  void set_zoneout_hidden(float zoneout_hidden);
  float get_zoneout_hidden() const;
  int64_t get_good_ld(const int64_t dim, const int64_t type_size);
};
AbstractBasePtr LstmGradInfer(const abstract::AnalysisEnginePtr &, const PrimitivePtr &primitive,
                              const std::vector<AbstractBasePtr> &input_args);
}  // namespace ops
}  // namespace luojianet_ms

#endif  // LUOJIANET_MS_CORE_OPS_LSTM_GRAD_H_
