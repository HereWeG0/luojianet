/**
 * Copyright 2020 Huawei Technologies Co., Ltd
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
#include <iostream>
#include "gtest/gtest.h"
#include "utils/log_adapter.h"
#include "pipeline/jit/pipeline.h"
#include "pipeline/jit/resource.h"

namespace luojianet_ms {
  extern void InitSubModulesLogLevel();
}

GTEST_API_ int main(int argc, char** argv) {
  luojianet_ms::InitSubModulesLogLevel();
  testing::InitGoogleTest(&argc, argv);
  int ret = RUN_ALL_TESTS();
  luojianet_ms::pipeline::ClearResAtexit();
  return ret;
}
