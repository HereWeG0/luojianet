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

#ifndef LUOJIANET_MS_CCSRC_UTILS_COMM_MANAGER_H
#define LUOJIANET_MS_CCSRC_UTILS_COMM_MANAGER_H

#include <string>
#include <vector>
#include <utility>
#include "utils/log_adapter.h"

using std::string;
using std::vector;

namespace luojianet_ms {
constexpr unsigned int NO_COMM_DLIB_RANK_SIZE = 2048;

class CommManager {
 public:
  static CommManager &GetInstance() noexcept;
  bool CreateGroupSync(const string &group, const vector<unsigned int> &rank_id_list) const;
  bool DestroyGroup(const string &group) const;
  bool GetRankID(const string &group, unsigned int *rank_id) const;
  bool GetRankSize(const string &group, unsigned int *rank_size) const;
  ~CommManager() = default;

  CommManager(const CommManager &) = delete;

 private:
  explicit CommManager(string backend) : backend_(std::move(backend)) {}
  string backend_;
};

uint32_t GetRank();

bool IsStandAlone();
}  // namespace luojianet_ms
#endif  // LUOJIANET_MS_CCSRC_UTILS_COMMUNICATION_MANAGER_H
