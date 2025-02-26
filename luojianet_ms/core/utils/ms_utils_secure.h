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
#ifndef LUOJIANET_MS_CORE_UTILS_MS_UTILS_SECURE_H_
#define LUOJIANET_MS_CORE_UTILS_MS_UTILS_SECURE_H_

#include "securec/include/securec.h"
#include "utils/log_adapter.h"

namespace luojianet_ms {
namespace common {
static inline errno_t huge_memcpy_s(uint8_t *destAddr, size_t destMaxLen, const uint8_t *srcAddr, size_t srcLen) {
  MS_EXCEPTION_IF_NULL(destAddr);
  MS_EXCEPTION_IF_NULL(srcAddr);
  auto dest = destAddr;
  auto src = srcAddr;
  auto destMax = destMaxLen;
  auto count = srcLen;

  while (destMax > SECUREC_MEM_MAX_LEN && count > SECUREC_MEM_MAX_LEN) {
    auto ret = memcpy_s(dest, SECUREC_MEM_MAX_LEN, src, SECUREC_MEM_MAX_LEN);
    if (ret != 0) {
      return ret;
    }
    dest += SECUREC_MEM_MAX_LEN;
    src += SECUREC_MEM_MAX_LEN;
    destMax -= SECUREC_MEM_MAX_LEN;
    count -= SECUREC_MEM_MAX_LEN;
  }
  return memcpy_s(dest, destMax, src, count);
}
}  // namespace common
}  // namespace luojianet_ms

#endif  // LUOJIANET_MS_CORE_UTILS_MS_UTILS_SECURE_H_
