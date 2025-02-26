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
#ifdef ENABLE_ARM
#include <string>
#ifndef LUOJIANET_MS_LITE_SRC_CPU_INFO_H
#define LUOJIANET_MS_LITE_SRC_CPU_INFO_H
namespace luojianet_ms::lite {
#ifndef MS_COMPILE_IOS
#define ARM_CPU_IMPLEMENTER_MASK UINT32_C(0xFF000000)
#define ARM_CPU_PART_MASK UINT32_C(0x0000FFF0)
#define ARM_CPU_IMPLEMENTER_OFFSET 24
#define ARM_CPU_PART_OFFSET 4
typedef struct AndroidCpuInfo {
  uint32_t cpu_implementer = 0;
  uint32_t cpu_part = 0;
  std::string hardware = "";
} AndroidCpuInfo;
#endif

class CpuInfo {
 public:
  CpuInfo() = default;
  virtual ~CpuInfo() = default;
  bool ArmIsSupportFp16();

 private:
#ifndef MS_COMPILE_IOS
  uint32_t StringToDigit(const std::string &str);
  uint32_t ParseArmCpuPart(const std::string &suffix);
  uint32_t MidrSetImplementer(uint32_t implementer);
  uint32_t MidrSetPart(uint32_t part);
  uint32_t ParseArmCpuImplementer(const std::string &suffix);
  void GetArmProcCpuInfo(AndroidCpuInfo *android_cpu_info);
  uint32_t midr_ = 0;
  AndroidCpuInfo android_cpu_info_;
#endif
  bool fp16_flag_ = false;
};
}  // namespace luojianet_ms::lite
#endif  // LUOJIANET_MS_LITE_SRC_CPU_INFO_H
#endif
