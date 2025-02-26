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

#ifndef LUOJIANET_MS_LITE_LOAD_HOST_CACHE_DATA_H_
#define LUOJIANET_MS_LITE_LOAD_HOST_CACHE_DATA_H_

#include <map>
#include <string>
#include "include/api/status.h"
#include "include/api/data_type.h"
#include "include/api/types.h"
#include "include/api/kernel.h"
#include "include/api/delegate.h"
#include "src/lite_model.h"

namespace luojianet_ms {
namespace cache {
class HostCacheModel {
 public:
  HostCacheModel() {}
  Status LoadCache(const std::string &model_path);
  Status LoadCache(DelegateModel<schema::Primitive> *model);
  bool CheckIsCacheKernel(kernel::Kernel *kernel);
  MSTensor GetHostCacheTensor(kernel::Kernel *kernel);

 private:
  std::map<std::string, MSTensor> cache_tensor_;
  luojianet_ms::lite::LiteModel *cache_model_{nullptr};
  char *model_buf_{nullptr};
  size_t model_size_;
};
}  // namespace cache
}  // namespace luojianet_ms
#endif  // LUOJIANET_MS_LITE_EMBEDDING_CACHE_H_
