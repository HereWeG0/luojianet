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
#ifndef LUOJIANET_MS_LITE_SRC_CXX_API_CONTEXT_H_
#define LUOJIANET_MS_LITE_SRC_CXX_API_CONTEXT_H_

#include <cstdint>
#include <map>
#include <vector>
#include <string>
#include <memory>
#ifndef SUPPORT_NNIE
#include <any>
#else
#include <experimental/any>
#endif
#include "include/api/context.h"

namespace luojianet_ms {
struct Context::Data {
  std::vector<std::shared_ptr<DeviceInfoContext>> device_info_list;
  int32_t thread_num = 2;
  bool enable_parallel_ = false;
  std::vector<int32_t> affinity_core_list_;
  int affinity_mode_ = 0;
  std::shared_ptr<Delegate> delegate = nullptr;
};

struct DeviceInfoContext::Data {
#ifndef SUPPORT_NNIE
  std::map<std::string, std::any> params;
#else
  std::map<std::string, std::experimental::any> params;
#endif
  std::shared_ptr<Allocator> allocator = nullptr;
};
}  // namespace luojianet_ms

#endif  // LUOJIANET_MS_LITE_SRC_CXX_API_CONTEXT_H_
