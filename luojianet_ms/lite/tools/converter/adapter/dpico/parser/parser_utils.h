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

#ifndef LUOJIANET_MS_LITE_TOOLS_CONVERTER_DPICO_PARSER_PARSER_UTILS_H
#define LUOJIANET_MS_LITE_TOOLS_CONVERTER_DPICO_PARSER_PARSER_UTILS_H

#include <set>
#include <string>
#include "ir/anf.h"
#include "include/api/format.h"
#include "api/ir/func_graph.h"
#include "utils/log_adapter.h"
#include "include/errorcode.h"
#include "google/protobuf/io/zero_copy_stream_impl.h"
#include "google/protobuf/text_format.h"
#include "google/protobuf/io/coded_stream.h"
using luojianet_ms::lite::STATUS;
namespace luojianet_ms {
namespace lite {
void GetAllFuncGraph(const api::FuncGraphPtr &func_graph, std::set<api::FuncGraphPtr> *all_func_graphs);
int PostAdjust(const std::set<api::FuncGraphPtr> &all_func_graphs);
int UnifyConvWeightFormat(const api::FuncGraphPtr &graph, const CNodePtr &cnode, luojianet_ms::Format src_format,
                          luojianet_ms::Format dst_format, std::set<AnfNodePtr> *has_visited);
bool ReadProtoFromCodedInputStream(google::protobuf::io::CodedInputStream *coded_stream,
                                   google::protobuf::Message *proto);
int ReadProtoFromText(const char *file, google::protobuf::Message *message);
int ReadProtoFromBinaryFile(const char *file, google::protobuf::Message *message);
STATUS ValidateFileStr(const std::string &modelFile, const std::string &fileType);
}  // namespace lite
}  // namespace luojianet_ms

#endif  // LUOJIANET_MS_LITE_TOOLS_CONVERTER_DPICO_PARSER_PARSER_UTILS_H
