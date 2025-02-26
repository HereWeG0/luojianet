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

#ifndef LUOJIANET_MS_CCSRC_BACKEND_KERNEL_COMPILER_CPU_LSTM_GRAD_CPU_KERNEL_H_
#define LUOJIANET_MS_CCSRC_BACKEND_KERNEL_COMPILER_CPU_LSTM_GRAD_CPU_KERNEL_H_

#include <string>
#include <vector>
#include <memory>
#include "backend/kernel_compiler/cpu/mkldnn/mkl_cpu_kernel.h"

namespace luojianet_ms {
namespace kernel {
class LSTMGradCPUKernel : public MKLCPUKernel {
 public:
  LSTMGradCPUKernel() = default;
  ~LSTMGradCPUKernel() override = default;

  void InitKernel(const CNodePtr &kernel_node) override;
  bool Launch(const std::vector<AddressPtr> &inputs, const std::vector<AddressPtr> &workspace,
              const std::vector<AddressPtr> &outputs) override;

 protected:
  void InitInputOutputSize(const CNodePtr &kernel_node) override;

 private:
  void AddArgumentOp(const dnnl::memory::desc &src_desc, const dnnl::memory::desc &src_h_desc,
                     const dnnl::memory::desc &src_c_desc, const dnnl::memory::desc &bias_desc,
                     const dnnl::memory::desc &dst_desc, const dnnl::memory::desc &dst_h_desc,
                     const dnnl::memory::desc &dst_c_desc);
  void SetArgumentHandleOp(const std::vector<kernel::AddressPtr> &inputs,
                           const std::vector<kernel::AddressPtr> &outputs, const dnnl::memory &weights_memory,
                           const dnnl::memory &weights_h_memory, const dnnl::memory &bias_memory,
                           const dnnl::memory &diff_weights_memory, const dnnl::memory &diff_weights_h_memory,
                           const dnnl::memory &diff_bias_memory);
  void ResetMemory(const dnnl::memory &mem, const string name) const;
  void CheckParam(const CNodePtr &kernel_node);

  int num_directions_{0};
  bool bidirectional_{false};
  bool has_bias_{false};
  int64_t weight_size_{0};
  int64_t weight_h_size_{0};
  int64_t input_size_{0};
  int64_t hidden_size_{0};
  int64_t num_layers_{0};
  int64_t batch_size_{0};
  int64_t seq_len_{0};
  size_t reserve_size_{0};

  dnnl::memory::dims weights_dims_;
  dnnl::memory::dims weights_h_dims_;
  dnnl::memory::dims bias_dims_;
  dnnl::lstm_backward::primitive_desc prim_backward_desc_;
};

MS_REG_CPU_KERNEL(LSTMGrad,
                  KernelAttr()
                    .AddInputAttr(kNumberTypeFloat32)
                    .AddInputAttr(kNumberTypeFloat32)
                    .AddInputAttr(kNumberTypeFloat32)
                    .AddInputAttr(kNumberTypeFloat32)
                    .AddInputAttr(kNumberTypeFloat32)
                    .AddInputAttr(kNumberTypeFloat32)
                    .AddInputAttr(kNumberTypeFloat32)
                    .AddInputAttr(kNumberTypeFloat32)
                    .AddInputAttr(kNumberTypeFloat32)
                    .AddInputAttr(kNumberTypeFloat32)
                    .AddInputAttr(kNumberTypeFloat32)
                    .AddOutputAttr(kNumberTypeFloat32)
                    .AddOutputAttr(kNumberTypeFloat32)
                    .AddOutputAttr(kNumberTypeFloat32)
                    .AddOutputAttr(kNumberTypeFloat32),
                  LSTMGradCPUKernel);
}  // namespace kernel
}  // namespace luojianet_ms
#endif  // LUOJIANET_MS_CCSRC_BACKEND_KERNEL_COMPILER_CPU_LSTM_GRAD_CPU_KERNEL_H_
