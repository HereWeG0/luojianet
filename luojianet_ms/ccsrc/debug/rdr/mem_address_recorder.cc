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
#include "debug/rdr/mem_address_recorder.h"
#include <fstream>
#include <sstream>
#include <utility>
#include "backend/kernel_compiler/kernel.h"

namespace luojianet_ms {
namespace {
std::string MemInfo2String(const std::string &label, const AddressPtrList &info) {
  std::ostringstream ss;
  ss << label << " " << info.size() << std::endl;
  for (size_t i = 0; i < info.size(); i++) {
    if (info[i] != nullptr) {
      ss << "&" << info[i]->addr << " #" << info[i]->size << std::endl;
    }
  }
  return ss.str();
}
}  // namespace

void MemAddressRecorder::SaveMemInfo(const std::string &op_name, const kernel::KernelLaunchInfo &mem_info) {
  std::lock_guard<std::mutex> lock(mtx_);
  if (!printed_) {
    MS_LOG(INFO) << "RDR update mem info.";
    printed_ = true;
  }
  if (op_names_.count(op_name) != 0) {
    op_names_.clear();
    mem_info_stream_.str("");
  }
  op_names_.insert(op_name);
  mem_info_stream_ << op_name << std::endl;
  mem_info_stream_ << MemInfo2String("kernel_inputs", mem_info.inputs_);
  mem_info_stream_ << MemInfo2String("kernel_workspaces", mem_info.workspaces_);
  mem_info_stream_ << MemInfo2String("kernel_outputs", mem_info.outputs_);
  mem_info_stream_ << std::endl;
}

void MemAddressRecorder::Export() {
  auto realpath = GetFileRealPath();
  if (!realpath.has_value()) {
    return;
  }
  std::lock_guard<std::mutex> lock(mtx_);
  std::string file_path = realpath.value() + ".txt";
  ChangeFileMode(file_path, S_IRWXU);
  std::ofstream fout(file_path);
  if (!fout.is_open()) {
    MS_LOG(WARNING) << "Open file for saving memory information failed. File path: '" << file_path << "'.";
    return;
  }
  MS_LOG(INFO) << "RDR export device memory information.";
  fout << mem_info_stream_.str();
  fout.close();
  ChangeFileMode(file_path, S_IRUSR);
}

void MemAddressRecorder::CleanUp() {
  std::lock_guard<std::mutex> lock(mtx_);
  MS_LOG(INFO) << "RDR clean up mem info, kernel size equals " << op_names_.size();
  op_names_.clear();
  mem_info_stream_.str("");
  printed_ = false;
}
}  // namespace luojianet_ms
