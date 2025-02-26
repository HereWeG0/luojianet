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

#ifndef LUOJIANET_MS_CCSRC_FRONTEND_PARALLEL_GRAPH_UTIL_GET_GRAPH_INFO_H_
#define LUOJIANET_MS_CCSRC_FRONTEND_PARALLEL_GRAPH_UTIL_GET_GRAPH_INFO_H_

#include "pybind11/stl.h"
#include "pybind11/pybind11.h"
#include "ir/anf.h"
#include "pipeline/jit/resource.h"

namespace py = pybind11;
namespace luojianet_ms {
namespace parallel {
py::dict GetParameterLayoutFromGraph(const FuncGraphPtr &graph);
py::dict GetParameterLayoutFromResource(const pipeline::ResourcePtr &resource);
py::dict GetAllreduceFusion(const FuncGraphPtr &graph);
py::list GetParallelParameterNameListFromGraph(const FuncGraphPtr &graph);
py::list GetParallelParameterNameListFromResource(const pipeline::ResourcePtr &resource);
}  // namespace parallel
}  // namespace luojianet_ms

#endif  // LUOJIANET_MS_CCSRC_FRONTEND_PARALLEL_GRAPH_UTIL_GET_GRAPH_INFO_H_
