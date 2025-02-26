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

#ifndef H13960AED_B5B1_45F9_A664_6CB6C15CA3C1
#define H13960AED_B5B1_45F9_A664_6CB6C15CA3C1

#include "easy_graph/infra/keywords.h"
#include "easy_graph/infra/status.h"

EG_NS_BEGIN

struct Graph;
struct Node;
struct Edge;

INTERFACE(GraphVisitor) {
  DEFAULT(Status, Visit(const Graph &));
  DEFAULT(Status, Visit(const Node &));
  DEFAULT(Status, Visit(const Edge &));
};

EG_NS_END

#endif
