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

#include "runtime/hardware/collective/collective_communication_lib.h"

namespace luojianet_ms {
namespace device {
bool CollectiveCommunicationLib::Finalize() {
  if (!initialized_) {
    return true;
  }

  for (const auto &group : groups_) {
    CHECK_IF_NULL(group.second);
    if (!group.second->Finalize()) {
      MS_LOG(ERROR) << "Failed to finalize group " << group.first;
      return false;
    }
  }
  groups_.clear();
  initialized_ = false;
  return true;
}

bool CollectiveCommunicationLib::DestroyCommunicationGroup(const std::string &group_name) {
  if (groups_.count(group_name) == 0) {
    MS_LOG(ERROR) << "The group " << group_name << " does not exist.";
    return false;
  }
  auto group = groups_[group_name];
  CHECK_IF_NULL(group);
  if (!group->Finalize()) {
    MS_LOG(ERROR) << "Failed to finalize group " << group_name;
    return false;
  }
  (void)groups_.erase(group_name);
  return true;
}

uint32_t CollectiveCommunicationLib::GetRankId(const std::string &group_name) {
  CHECK_RET(groups_.count(group_name) != 0, true, "The group " + group_name + " does not exist.");
  auto group = groups_[group_name];
  CHECK_IF_NULL(group);
  return group->GetGroupRank(global_rank_id_);
}

uint32_t CollectiveCommunicationLib::GetGroupSize(const std::string &group_name) {
  CHECK_RET(groups_.count(group_name) != 0, true, "The group " + group_name + " does not exist.");
  auto group = groups_[group_name];
  CHECK_IF_NULL(group);
  return group->group_size();
}

CommunicationGroupPtr CollectiveCommunicationLib::GetGroup(const std::string &group_name) {
  if (groups_.count(group_name) == 0) {
    MS_LOG(ERROR) << "The group " << group_name << " does not exist.";
    return nullptr;
  }
  return groups_[group_name];
}

const std::string &CollectiveCommunicationLib::global_group_name() const { return global_group_name_; }

uint32_t CollectiveCommunicationLib::global_rank_id() const { return global_rank_id_; }

uint32_t CollectiveCommunicationLib::local_rank_id() const { return local_rank_id_; }

uint32_t CollectiveCommunicationLib::global_rank_size() const { return global_rank_size_; }
}  // namespace device
}  // namespace luojianet_ms
