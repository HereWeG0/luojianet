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

#ifndef DPICO_CHECKER_REDUCE_CHECKER_H_
#define DPICO_CHECKER_REDUCE_CHECKER_H_

#include <string>
#include "checker/op_checker.h"

namespace luojianet_ms {
namespace dpico {
class ReduceChecker : public OpChecker {
 public:
  ReduceChecker() : OpChecker("Reduce") {}
  ~ReduceChecker() override = default;
  bool Check(CNodePtr op, int32_t output_num, luojianet_ms::Format format) override;
};
}  // namespace dpico
}  // namespace luojianet_ms

#endif  // DPICO_CHECKER_REDUCE_CHECKER_H_
