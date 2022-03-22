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
#ifndef LUOJIANET_MS_CCSRC_DEBUG_RDR_RUNNING_DATA_RECORDER_H_
#define LUOJIANET_MS_CCSRC_DEBUG_RDR_RUNNING_DATA_RECORDER_H_
#include <vector>
#include <string>
#include <memory>

#include "luojianet_ms/core/utils/log_adapter.h"
#include "debug/rdr/graph_recorder.h"

namespace luojianet_ms {
class FuncGraph;
class CNode;
using FuncGraphPtr = std::shared_ptr<FuncGraph>;
using CNodePtr = std::shared_ptr<CNode>;

namespace kernel {
class Address;
struct KernelLaunchInfo;
using AddressPtr = std::shared_ptr<Address>;
}  // namespace kernel
using AddressPtrList = std::vector<kernel::AddressPtr>;
struct MemInfo;
#ifdef ENABLE_D
namespace device {
namespace ascend {
namespace tasksink {
class TaskDebugInfo;
}  // namespace tasksink
}  // namespace ascend
}  // namespace device
using TaskDebugInfoPtr = std::shared_ptr<device::ascend::tasksink::TaskDebugInfo>;
#endif  // ENABLE_D

namespace RDR {
bool RecordAnfGraph(const SubModuleId module, const std::string &name, const FuncGraphPtr &graph,
                    const DumpGraphParams &info, const std::string &file_type = ".ir;.pb;.dat");
bool RecordGraphExecOrder(const SubModuleId module, const std::string &name,
                          const std::vector<CNodePtr> &final_exec_order);
bool RecordString(SubModuleId module, const std::string &name, const std::string &data);
bool RecordStreamExecOrder(const SubModuleId module, const std::string &name, const std::vector<CNodePtr> &exec_order);
bool RecordMemAddressInfo(const SubModuleId module, const std::string &name);
bool UpdateMemAddress(const SubModuleId module, const std::string &name, const std::string &op_name,
                      const kernel::KernelLaunchInfo &mem_info);
#ifdef ENABLE_D
bool RecordTaskDebugInfo(SubModuleId module, const std::string &name,
                         const std::vector<TaskDebugInfoPtr> &task_debug_info_list);
#endif  // ENABLE_D
void TriggerAll();
void Snapshot();
void ResetRecorder();
void ClearMemAddressInfo();
}  // namespace RDR
}  // namespace luojianet_ms
#endif  // LUOJIANET_MS_CCSRC_DEBUG_RDR_RUNNING_DATA_RECORDER_H_
