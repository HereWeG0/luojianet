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

#ifndef LUOJIANET_MS_CCSRC_RUNTIME_DEVICE_GPU_MPI_MPI_INITIALIZER_H_
#define LUOJIANET_MS_CCSRC_RUNTIME_DEVICE_GPU_MPI_MPI_INITIALIZER_H_

#include <string>
#include "runtime/device/gpu/distribution/collective_wrapper.h"

namespace luojianet_ms {
namespace device {
namespace gpu {
class MPIInitializer {
 public:
  MPIInitializer(MPIInitializer const &) = delete;
  MPIInitializer &operator=(const MPIInitializer &) = delete;
  static MPIInitializer &GetInstance();
  static int get_rank_id(const std::string &group);
  static int get_rank_size(const std::string &groups);

 private:
  MPIInitializer() = default;
  ~MPIInitializer() = default;
};
}  // namespace gpu
}  // namespace device
}  // namespace luojianet_ms

#endif  // LUOJIANET_MS_CCSRC_RUNTIME_DEVICE_GPU_MPI_MPI_INITIALIZER_H_
