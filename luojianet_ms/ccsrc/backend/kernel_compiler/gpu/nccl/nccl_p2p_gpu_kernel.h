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

#ifndef LUOJIANET_MS_CCSRC_BACKEND_KERNEL_COMPILER_GPU_NCCL_P2P_GPU_KERNEL_H_
#define LUOJIANET_MS_CCSRC_BACKEND_KERNEL_COMPILER_GPU_NCCL_P2P_GPU_KERNEL_H_

#include <dlfcn.h>
#include <stdint.h>
#include <vector>
#include <string>
#include <map>
#include "backend/kernel_compiler/gpu/nccl/nccl_gpu_kernel.h"

namespace luojianet_ms {
namespace kernel {
enum NcclKernelType { NCCL_ALLTOALLV = 0, NCCL_NEIGHBOREXCHANGE = 1, NCCL_INVALID_TYPE = 255 };
const std::map<std::string, NcclKernelType> kNcclTypeMap = {{"AllToAllv", NCCL_ALLTOALLV},
                                                            {"NeighborExchange", NCCL_NEIGHBOREXCHANGE}};

template <typename T, typename I>
class NcclP2PGpuKernel : public NcclGpuKernel {
 public:
  NcclP2PGpuKernel() { ResetResource(); }
  ~NcclP2PGpuKernel() override = default;

  const std::vector<size_t> &GetInputSizeList() const override { return input_size_list_; }
  const std::vector<size_t> &GetOutputSizeList() const override { return output_size_list_; }
  const std::vector<size_t> &GetWorkspaceSizeList() const override { return workspace_size_list_; }

  bool Launch(const std::vector<AddressPtr> &inputs, const std::vector<AddressPtr> &,
              const std::vector<AddressPtr> &outputs, void *stream_ptr) override {
    switch (nccl_kernel_type_) {
      case NCCL_ALLTOALLV: {
        LaunchAllToAllv(inputs, outputs, stream_ptr);
        break;
      }
      case NCCL_NEIGHBOREXCHANGE: {
        LaunchAllToAllv(inputs, outputs, stream_ptr);
        break;
      }
      default: {
        MS_LOG(EXCEPTION) << "Kernel type " << nccl_kernel_type_ << " is not supported.";
      }
    }
    return true;
  }

  bool Init(const CNodePtr &kernel_node) override {
    MS_EXCEPTION_IF_NULL(kernel_node);
    kernel_node_ = kernel_node;
    InferCommType(kernel_node);

    size_t input_num = AnfAlgo::GetInputTensorNum(kernel_node);
    size_t output_num = AnfAlgo::GetOutputTensorNum(kernel_node);
    if (input_num > 0) {
      input_nccl_data_type_ = nccl_dtype(AnfAlgo::GetInputDeviceDataType(kernel_node, 0));
    }
    if (output_num > 0) {
      output_nccl_data_type_ = nccl_dtype(AnfAlgo::GetOutputDeviceDataType(kernel_node, 0));
    }
    for (size_t i = 0; i < input_num; ++i) {
      auto shape = AnfAlgo::GetInputRealDeviceShapeIfExist(kernel_node, i);
      is_null_input_ = CHECK_NULL_INPUT(shape);
      if (is_null_input_) {
        MS_LOG(WARNING) << "For 'NcclP2PGpuKernel', input shape is null ";
        InitSizeLists();
        return true;
      }
      size_t size = sizeof(T);
      for (size_t j = 0; j < shape.size(); j++) {
        size *= IntToSize(shape[j]);
      }
      input_size_list_.push_back(size);
      input_size_ += size;
    }
    for (size_t i = 0; i < output_num; ++i) {
      auto shape = AnfAlgo::GetOutputRealDeviceShapeIfExist(kernel_node, i);
      is_null_input_ = CHECK_NULL_INPUT(shape);
      if (is_null_input_) {
        MS_LOG(WARNING) << "For 'NcclP2PGpuKernel', output shape is null";
        InitSizeLists();
        return true;
      }
      size_t size = sizeof(I);
      for (size_t j = 0; j < shape.size(); j++) {
        size *= IntToSize(shape[j]);
      }
      output_size_list_.push_back(size);
      output_size_ += size;
    }

    group_name_ = GetAttr<std::string>(kernel_node, kAttrGroup);
    MS_LOG(INFO) << AnfAlgo::GetCNodeName(kernel_node) << " for group " << group_name_;
    auto prim = AnfAlgo::GetCNodePrimitive(kernel_node);
    MS_EXCEPTION_IF_NULL(prim);
    auto comm_stream_attr = prim->GetAttr("stream_id");
    if (comm_stream_attr) {
      comm_stream_ = reinterpret_cast<cudaStream_t>(GetValue<uintptr_t>(comm_stream_attr));
      MS_EXCEPTION_IF_NULL(comm_stream_);
    }

    // Used by AlltoAllv
    auto send_rank_ids_attr = prim->GetAttr(kAttrSendRankIds);
    auto recv_rank_ids_attr = prim->GetAttr(kAttrRecvRankIds);
    if (send_rank_ids_attr && recv_rank_ids_attr) {
      send_rank_ids = GetValue<std::vector<int64_t>>(send_rank_ids_attr);
      recv_rank_ids = GetValue<std::vector<int64_t>>(recv_rank_ids_attr);
    }

    use_mpi_ = common::CheckUseMPI();
    if (use_mpi_) {
      collective_handle_ = device::gpu::CollectiveInitializer::instance().collective_handle();
      MS_EXCEPTION_IF_NULL(collective_handle_);
    }
    return true;
  }

  void ResetResource() noexcept override {
    nccl_kernel_type_ = NCCL_INVALID_TYPE;
    input_size_ = 0;
    output_size_ = 0;
    root_ = 0;
    is_null_input_ = false;
    collective_handle_ = nullptr;
    comm_stream_ = nullptr;
    input_size_list_.clear();
    output_size_list_.clear();
    workspace_size_list_.clear();
  }

 protected:
  void InitSizeLists() override { return; }

 private:
  void LaunchAllToAllv(const std::vector<AddressPtr> &inputs, const std::vector<AddressPtr> &outputs,
                       void *stream_ptr) {
    T *input_addr = nullptr;
    I *output_addr = nullptr;
    cudaStream_t stream = comm_stream_ ? comm_stream_ : reinterpret_cast<cudaStream_t>(stream_ptr);

    // send_rank_id and recv rank_id size needs to be equal to input_list size
    if (send_rank_ids.size() != input_size_list_.size()) {
      MS_LOG(ERROR) << "Trying to use AlltoAllv, but send_rank_ids vector size not equals to input_list size.";
    }
    if (recv_rank_ids.size() != output_size_list_.size()) {
      MS_LOG(ERROR) << "Trying to use AlltoAllv, but recv_rank_ids vector size not equals to output_list size.";
    }

    // This implementation refers to NVIDIA NCCL 2.11 doc.
    (void)GroupStart();
    for (int i = 0; i < SizeToInt(input_size_list_.size()); ++i) {
      input_addr = GetDeviceAddress<T>(inputs, i);
      (void)Send(input_addr, input_size_list_[i] / sizeof(T), input_nccl_data_type_, send_rank_ids[i], stream,
                 group_name_);
    }
    for (int i = 0; i < SizeToInt(output_size_list_.size()); ++i) {
      output_addr = GetDeviceAddress<I>(outputs, i);
      (void)Recv(output_addr, output_size_list_[i] / sizeof(I), output_nccl_data_type_, recv_rank_ids[i], stream,
                 group_name_);
    }
    (void)GroupEnd();
  }

  void InferCommType(const CNodePtr &kernel_node) {
    std::string kernel_name = AnfAlgo::GetCNodeName(kernel_node);
    auto iter = kNcclTypeMap.find(kernel_name);
    if (iter == kNcclTypeMap.end()) {
      MS_LOG(EXCEPTION) << "Kernel " << kernel_name << " is not supported.";
    } else {
      nccl_kernel_type_ = iter->second;
    }

    auto prim = AnfAlgo::GetCNodePrimitive(kernel_node);
    MS_EXCEPTION_IF_NULL(prim);

    auto root_rank = prim->GetAttr(kAttrRootRank);
    if (root_rank) {
      root_ = static_cast<int>(GetValue<int64_t>(root_rank));
    }
    return;
  }

  std::vector<size_t> input_size_list_;
  std::vector<size_t> output_size_list_;
  std::vector<size_t> workspace_size_list_;
  NcclKernelType nccl_kernel_type_;
  size_t input_size_;
  size_t output_size_;
  int root_;
  bool is_null_input_;
  cudaStream_t comm_stream_;
  ncclDataType_t output_nccl_data_type_;
  ncclDataType_t input_nccl_data_type_;
  std::vector<int64_t> send_rank_ids;
  std::vector<int64_t> recv_rank_ids;
};
}  // namespace kernel
}  // namespace luojianet_ms

#endif  // LUOJIANET_MS_CCSRC_BACKEND_KERNEL_COMPILER_GPU_NCCL_P2P_GPU_KERNEL_H_
