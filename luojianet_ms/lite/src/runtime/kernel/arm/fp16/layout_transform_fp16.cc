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
#include "src/runtime/kernel/arm/fp16/layout_transform_fp16.h"
#include "nnacl/fp16/pack_fp16.h"
#include "src/common/log_adapter.h"
#include "schema/ops_types_generated.h"

namespace luojianet_ms::kernel {
LayoutConvertor LayoutTransformFp16(luojianet_ms::Format src_format, luojianet_ms::Format dst_format) {
  if (src_format == luojianet_ms::NHWC && dst_format == luojianet_ms::NC4HW4) {
    return PackNHWCToNC4HW4Fp16;
  } else if (src_format == luojianet_ms::NHWC && dst_format == luojianet_ms::NHWC4) {
    return PackNHWCToNHWC4Fp16;
  } else if (src_format == luojianet_ms::NC4HW4 && dst_format == luojianet_ms::NHWC4) {
    return PackNC4HW4ToNHWC4Fp16;
  } else if (src_format == luojianet_ms::NCHW && dst_format == luojianet_ms::NC4HW4) {
    return PackNCHWToNC4HW4Fp16;
  } else if (src_format == luojianet_ms::NC4HW4 && dst_format == luojianet_ms::NHWC) {
    return PackNC4HW4ToNHWCFp16;
  } else {
    MS_LOG(ERROR) << "Unsupported transform from " << EnumNameFormat(static_cast<schema::Format>(src_format)) << " to "
                  << EnumNameFormat(static_cast<schema::Format>(dst_format));
    return nullptr;
  }
}
}  // namespace luojianet_ms::kernel
