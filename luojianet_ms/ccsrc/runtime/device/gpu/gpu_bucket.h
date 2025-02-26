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

#ifndef LUOJIANET_MS_LUOJIANET_MS_CCSRC_RUNTIME_DEVICE_GPU_GPU_BUCKET_H_
#define LUOJIANET_MS_LUOJIANET_MS_CCSRC_RUNTIME_DEVICE_GPU_GPU_BUCKET_H_

#include <memory>
#include <vector>
#include "runtime/device/bucket.h"

namespace luojianet_ms::device::gpu {
class GPUBucket : public Bucket {
 public:
  GPUBucket(uint32_t id, uint32_t bucket_size);
  ~GPUBucket() override = default;

  void Init(const std::vector<void *> &compute_streams, const std::vector<void *> &communication_streams) override;

 private:
  void AllocateAllReduceAddr() override;
  void FreeAllDeviceMem() override;
  void FreeDeviceMem(void *dev_ptr) override;
  void CopyTensorToContiguousMemory() override;
  void LaunchAllReduce() override;
  std::shared_ptr<LaunchKernel> CreateLaunchMul() override;
  const void *collective_handle_;
};
}  // namespace luojianet_ms::device::gpu
#endif  // LUOJIANET_MS_LUOJIANET_MS_CCSRC_RUNTIME_DEVICE_GPU_GPU_BUCKET_H_
