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

#ifndef LUOJIANET_MS_CCSRC_MINDDATA_DATASET_KERNELS_IMAGE_RANDOM_SOLARIZE_OP_H
#define LUOJIANET_MS_CCSRC_MINDDATA_DATASET_KERNELS_IMAGE_RANDOM_SOLARIZE_OP_H

#include <memory>
#include <string>
#include <vector>

#include "minddata/dataset/core/tensor.h"
#include "minddata/dataset/kernels/tensor_op.h"
#include "minddata/dataset/util/status.h"
#include "minddata/dataset/kernels/image/solarize_op.h"
#include "minddata/dataset/util/random.h"

namespace luojianet_ms {
namespace dataset {
class RandomSolarizeOp : public SolarizeOp {
 public:
  // Pick a random threshold value to solarize the image with
  explicit RandomSolarizeOp(std::vector<uint8_t> threshold = {0, 255}) : threshold_(threshold) {
    rnd_.seed(GetSeed());
    is_deterministic_ = false;
  }

  ~RandomSolarizeOp() override = default;

  Status Compute(const std::shared_ptr<Tensor> &input, std::shared_ptr<Tensor> *output) override;

  std::string Name() const override { return kRandomSolarizeOp; }

 private:
  std::vector<uint8_t> threshold_;
  std::mt19937 rnd_;
};
}  // namespace dataset
}  // namespace luojianet_ms

#endif  // LUOJIANET_MS_CCSRC_MINDDATA_DATASET_KERNELS_IMAGE_RANDOM_SOLARIZE_OP_H
