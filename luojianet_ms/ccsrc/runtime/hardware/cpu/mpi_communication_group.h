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

#ifndef LUOJIANET_MS_CCSRC_DISTRIBUTED_COLLECTIVE_MPI_COMMUNICATION_GROUP_H_
#define LUOJIANET_MS_CCSRC_DISTRIBUTED_COLLECTIVE_MPI_COMMUNICATION_GROUP_H_

#include <mpi.h>
#include <string>
#include <vector>
#include <memory>
#include "runtime/hardware/collective/communication_group.h"
#include "utils/dlopen_macro.h"

namespace luojianet_ms {
namespace device {
namespace cpu {
class MPICommunicationGroup : public CommunicationGroup {
 public:
  explicit MPICommunicationGroup(const std::string &name, const std::vector<uint32_t> &group_ranks,
                                 uint32_t global_rank);

  ~MPICommunicationGroup() override = default;

  bool Initialize(void *root_info) override { return true; }
  bool Finalize() override;

  // The OpenMPI groups should be created from the world group.
  bool Initialize(const MPI_Group &world_group);

 private:
  MPI_Group group_;
  MPI_Comm group_communicator_;
};
using MPICommunicationGroupPtr = std::shared_ptr<MPICommunicationGroup>;
}  // namespace cpu
}  // namespace device
}  // namespace luojianet_ms
#endif  // LUOJIANET_MS_CCSRC_DISTRIBUTED_COLLECTIVE_MPI_COMMUNICATION_GROUP_H_
