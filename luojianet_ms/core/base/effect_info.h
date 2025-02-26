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

#ifndef LUOJIANET_MS_CORE_EFFECT_INFO_H_
#define LUOJIANET_MS_CORE_EFFECT_INFO_H_

namespace luojianet_ms {

struct EffectInfo {
  enum State : unsigned char {
    kUnknown = 0,
    kDetecting = 1,
    kDetected = 2,
  };
  State state = kUnknown;  // effect info state.
  bool memory = false;     // memory side effects, e.g., access global variable.
  bool io = false;         // IO side effects, e.g., print message.
  bool load = false;       // load value from global variable, e.g. add(self.para, x).

  void Merge(const EffectInfo &info) {
    if (info.state != EffectInfo::kDetected) {
      state = EffectInfo::kDetecting;
    }
    memory = memory || info.memory;
    io = io || info.io;
    load = load || info.load;
  }
};

// EffectInfoHolder as base class for effect info holders, such as CNode, FuncGraph, etc.
class EffectInfoHolder {
 public:
  // Gets effect info.
  const EffectInfo &GetEffectInfo() const { return effect_info_; }

  // Set effect info.
  void SetEffectInfo(const EffectInfo &info) { effect_info_ = info; }

  // Unset effect info.
  void UnsetEffectInfo() { effect_info_ = {EffectInfo::kUnknown, false, false}; }

 protected:
  EffectInfo effect_info_;
};

}  // namespace luojianet_ms

#endif  // LUOJIANET_MS_CORE_EFFECT_INFO_H_
