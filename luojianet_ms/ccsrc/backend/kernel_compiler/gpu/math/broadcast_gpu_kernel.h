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

#ifndef LUOJIANET_MS_CCSRC_BACKEND_KERNEL_COMPILER_GPU_MATH_BROADCAST_GPU_KERNEL_H_
#define LUOJIANET_MS_CCSRC_BACKEND_KERNEL_COMPILER_GPU_MATH_BROADCAST_GPU_KERNEL_H_

#include <cuda_runtime_api.h>
#include <vector>
#include <string>
#include <map>
#include "backend/kernel_compiler/gpu/gpu_kernel.h"
#include "backend/kernel_compiler/gpu/gpu_kernel_factory.h"
#include "backend/kernel_compiler/gpu/cuda_impl/broadcast_impl.cuh"
#include "backend/kernel_compiler/gpu/kernel_constants.h"
#include "backend/session/anf_runtime_algorithm.h"

namespace luojianet_ms {
namespace kernel {
constexpr int MAX_DIMS = 7;
template <typename T>
class BroadcastOpGpuKernel : public GpuKernel {
 public:
  BroadcastOpGpuKernel() { ResetResource(); }
  ~BroadcastOpGpuKernel() override = default;

  const std::vector<size_t> &GetInputSizeList() const override { return input_size_list_; }
  const std::vector<size_t> &GetOutputSizeList() const override { return output_size_list_; }
  const std::vector<size_t> &GetWorkspaceSizeList() const override { return workspace_size_list_; }

  bool Launch(const std::vector<AddressPtr> &inputs, const std::vector<AddressPtr> &,
              const std::vector<AddressPtr> &outputs, void *stream_ptr) override {
    if (is_null_input_) {
      return true;
    }
    T *lhs = GetDeviceAddress<T>(inputs, 0);
    T *rhs = GetDeviceAddress<T>(inputs, 1);

    if (is_comp_op_) {
      bool *output = GetDeviceAddress<bool>(outputs, 0);
      if (need_broadcast_) {
        BroadcastCmp(lhs_shape_, rhs_shape_, output_shape_, op_type_, lhs, rhs, output,
                     reinterpret_cast<cudaStream_t>(stream_ptr));
      } else {
        ElewiseCmp(output_num_, op_type_, lhs, rhs, output, reinterpret_cast<cudaStream_t>(stream_ptr));
      }
    } else {
      T *output = GetDeviceAddress<T>(outputs, 0);
      if (need_broadcast_) {
        BroadcastArith(lhs_shape_, rhs_shape_, output_shape_, op_type_, lhs, rhs, output,
                       reinterpret_cast<cudaStream_t>(stream_ptr));
      } else {
        ElewiseArith(output_num_, op_type_, lhs, rhs, output, reinterpret_cast<cudaStream_t>(stream_ptr));
      }
    }

    return true;
  }

  bool Init(const CNodePtr &kernel_node) override {
    GetOpType(kernel_node);
    auto shape1 = AnfAlgo::GetInputRealDeviceShapeIfExist(kernel_node, 0);
    auto shape2 = AnfAlgo::GetInputRealDeviceShapeIfExist(kernel_node, 1);
    auto shape3 = AnfAlgo::GetOutputRealDeviceShapeIfExist(kernel_node, 0);
    is_null_input_ = CHECK_NULL_INPUT(shape1) || CHECK_NULL_INPUT(shape2) || CHECK_NULL_INPUT(shape3);
    if (is_null_input_) {
      MS_LOG(WARNING) << "For 'BroadcastGpuKernel', input or output is null";
      InitSizeLists();
      return true;
    }
    need_broadcast_ = AnfAlgo::IsTensorBroadcast(shape1, shape2);
    if (need_broadcast_ && shape1.size() > MAX_DIMS) {
      MS_LOG(EXCEPTION) << "Broadcast operation not support dim greater than: " << MAX_DIMS << ", actual size is "
                        << shape1.size();
    }

    lhs_shape_.resize(MAX_DIMS, 1);
    rhs_shape_.resize(MAX_DIMS, 1);
    output_shape_.resize(MAX_DIMS, 1);
    for (size_t i = 0; i < shape3.size(); i++) {
      if (need_broadcast_) {
        if (i < MAX_DIMS) {
          output_shape_[i] = shape3[i];
        } else {
          MS_LOG(EXCEPTION) << "Output index: " << i << " should be less than " << MAX_DIMS;
        }
      }
      output_num_ *= shape3[i];
    }
    int lhs_offset = shape3.size() - shape1.size();
    for (size_t j = 0; j < shape1.size(); j++) {
      if (need_broadcast_) {
        if ((j + lhs_offset) >= 0 && (j + lhs_offset) < MAX_DIMS) {
          lhs_shape_[j + lhs_offset] = shape1[j];
        } else {
          auto index = j + lhs_offset;
          MS_LOG(EXCEPTION) << "Invalid input1 index: " << index;
        }
      }
      input1_num_ *= shape1[j];
    }
    int rhs_offset = shape3.size() - shape2.size();
    for (size_t k = 0; k < shape2.size(); k++) {
      if (need_broadcast_) {
        if ((k + rhs_offset) >= 0 && (k + rhs_offset) < MAX_DIMS) {
          rhs_shape_[k + rhs_offset] = shape2[k];
        } else {
          auto index = k + rhs_offset;
          MS_LOG(EXCEPTION) << "Invalid input2 index: " << index;
        }
      }
      input2_num_ *= shape2[k];
    }

    InitSizeLists();
    return true;
  }
  void ResetResource() noexcept override {
    op_type_ = BROADCAST_TYPE_INVALID;
    need_broadcast_ = false;
    is_comp_op_ = false;
    is_null_input_ = false;
    input1_num_ = 1;
    input2_num_ = 1;
    output_num_ = 1;
    lhs_shape_.clear();
    rhs_shape_.clear();
    output_shape_.clear();
    input_size_list_.clear();
    output_size_list_.clear();
    workspace_size_list_.clear();
  }

 protected:
  void InitResource() override { return; }
  void InitSizeLists() override {
    input_size_list_.push_back(input1_num_ * sizeof(T));
    input_size_list_.push_back(input2_num_ * sizeof(T));

    auto unit_size = is_comp_op_ ? sizeof(bool) : sizeof(T);
    output_size_list_.push_back(output_num_ * unit_size);
  }

 private:
  void GetOpType(const CNodePtr &kernel_node) {
    std::string kernel_name = AnfAlgo::GetCNodeName(kernel_node);

    static const std::map<std::string, BroadcastOpType> kBroadcastCmpTypeMap = {
      {"Greater", BROADCAST_TYPE_GREATER},
      {"Less", BROADCAST_TYPE_LESS},
      {"Equal", BROADCAST_TYPE_EQUAL},
      {"GreaterEqual", BROADCAST_TYPE_GREATER_EQUAL},
      {"LessEqual", BROADCAST_TYPE_LESS_EQUAL},
      {"NotEqual", BROADCAST_TYPE_NOT_EQUAL},
      {"LogicalAnd", BROADCAST_TYPE_LOGICAL_AND},
      {"LogicalOr", BROADCAST_TYPE_LOGICAL_OR},
    };

    auto iter = kBroadcastCmpTypeMap.find(kernel_name);
    if (iter != kBroadcastCmpTypeMap.end()) {
      op_type_ = iter->second;
      is_comp_op_ = true;
      return;
    }

    static const std::map<std::string, BroadcastOpType> kBroadcastArithmetricTypeMap = {
      {"Maximum", BROADCAST_TYPE_MAXIMUM},
      {"Minimum", BROADCAST_TYPE_MINIMUM},
      {"Pow", BROADCAST_TYPE_POWER},
      {"RealDiv", BROADCAST_TYPE_REALDIV},
      {"Mul", BROADCAST_TYPE_MUL},
      {"Sub", BROADCAST_TYPE_SUB},
      {"Add", BROADCAST_TYPE_ADD},
      {"FloorDiv", BROADCAST_TYPE_FLOORDIV},
      {"AbsGrad", BROADCAST_TYPE_ABSGRAD},
      {"Div", BROADCAST_TYPE_DIV},
      {"DivNoNan", BROADCAST_TYPE_DIVNONAN},
      {"Mod", BROADCAST_TYPE_MOD},
      {"FloorMod", BROADCAST_TYPE_FLOORMOD},
      {"Atan2", BROADCAST_TYPE_ATAN2},
      {"TruncateDiv", BROADCAST_TYPE_TRUNCATEDIV},
      {"TruncateMod", BROADCAST_TYPE_TRUNCATEMOD},
    };

    iter = kBroadcastArithmetricTypeMap.find(kernel_name);
    if (iter != kBroadcastArithmetricTypeMap.end()) {
      op_type_ = iter->second;
      is_comp_op_ = false;
      return;
    }

    MS_LOG(EXCEPTION) << "operation " << kernel_name << " is not supported.";
  }

  BroadcastOpType op_type_;
  bool need_broadcast_;
  bool is_comp_op_;
  bool is_null_input_;
  size_t input1_num_;
  size_t input2_num_;
  size_t output_num_;
  std::vector<size_t> lhs_shape_;
  std::vector<size_t> rhs_shape_;
  std::vector<size_t> output_shape_;

  std::vector<size_t> input_size_list_;
  std::vector<size_t> output_size_list_;
  std::vector<size_t> workspace_size_list_;
};
}  // namespace kernel
}  // namespace luojianet_ms

#endif  // LUOJIANET_MS_CCSRC_BACKEND_KERNEL_COMPILER_GPU_MATH_BROADCAST_GPU_KERNEL_H_
