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

#ifndef PYBIND_API_GIL_SCOPED_LONG_RUNNING_H_
#define PYBIND_API_GIL_SCOPED_LONG_RUNNING_H_

#include <memory>

#include "pybind11/pybind11.h"

#include "utils/scoped_long_running.h"

namespace py = pybind11;

namespace luojianet_ms {
class GilScopedLongRunningHook : public ScopedLongRunningHook {
 public:
  void Enter() override { release_ = std::make_unique<py::gil_scoped_release>(); }
  void Leave() override { release_ = nullptr; }

 private:
  std::unique_ptr<py::gil_scoped_release> release_;
};
}  // namespace luojianet_ms
#endif  // PYBIND_API_GIL_SCOPED_LONG_RUNNING_H_
