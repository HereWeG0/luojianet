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

#include "src/runtime/kernel/arm/fp32/lstm_fp32.h"
#include <cfloat>
#include <vector>
#include "schema/model_generated.h"
#include "src/kernel_registry.h"
#include "include/errorcode.h"
#include "nnacl/fp32/matmul_fp32.h"

using luojianet_ms::kernel::KERNEL_ARCH;
using luojianet_ms::lite::KernelRegistrar;
using luojianet_ms::lite::RET_ERROR;
using luojianet_ms::lite::RET_MEMORY_FAILED;
using luojianet_ms::lite::RET_OK;
using luojianet_ms::schema::PrimitiveType_LSTM;

namespace luojianet_ms::kernel {
void LstmCPUKernel::FreeTmpBuffer() {
  if (weight_i_ptr_ != nullptr) {
    free(weight_i_ptr_);
    weight_i_ptr_ = nullptr;
  }
  if (input_bias_ != nullptr) {
    free(input_bias_);
    input_bias_ = nullptr;
  }
#ifdef ENABLE_AVX
  if (weight_h_ptr_ != nullptr) {
    free(weight_h_ptr_);
    weight_h_ptr_ = nullptr;
  }
#else
  if (!state_is_vec_) {
    if (weight_h_ptr_ != nullptr) {
      free(weight_h_ptr_);
      weight_h_ptr_ = nullptr;
    }
  }
#endif
  if (state_bias_ != nullptr) {
    free(state_bias_);
    state_bias_ = nullptr;
  }
}

void LstmCPUKernel::FreeRunBuffer() {
  ms_context_->allocator->Free(buffer_[packed_input_index]);
  ms_context_->allocator->Free(buffer_[input_gate_index]);
  if (!state_is_vec_) {
    ms_context_->allocator->Free(buffer_[packed_state_index]);
  }
  ms_context_->allocator->Free(buffer_[state_gate_index]);
  if (!(lstm_param_->zoneout_cell_ >= -FLT_EPSILON && lstm_param_->zoneout_cell_ <= FLT_EPSILON)) {
    ms_context_->allocator->Free(buffer_[cell_state_index]);
  }
  if (!(lstm_param_->zoneout_hidden_ >= -FLT_EPSILON && lstm_param_->zoneout_hidden_ <= FLT_EPSILON)) {
    ms_context_->allocator->Free(buffer_[hidden_state_index]);
  }
  if (output_need_packed_) {
    ms_context_->allocator->Free(buffer_[avx_state_output_index]);
  }
}

int LstmCPUKernel::InitInputWeightBias() {
  // malloc and init input * weight right matrix buffer
  // input -- row: seq_len * batch; col: input_size
  // weight -- row: hidden_size; col: input_size, need transpose
  // result -- row: seq_len * batch; col: hidden_size
  weight_i_ptr_ = reinterpret_cast<float *>(
    malloc(weight_batch_ * lstm_param_->input_col_align_ * lstm_param_->input_size_ * sizeof(float)));
  if (weight_i_ptr_ == nullptr) {
    MS_LOG(ERROR) << "LstmCPUKernel malloc weight_i_ptr_ error.";
    return RET_ERROR;
  }
  int i_index = (in_tensors_.size() == mindir_input_tensors) ? combined_weights_index : onnx_weight_i_index;
  const int *weights_order = (in_tensors_.size() == mindir_input_tensors) ? weights_order_IFOG : nullptr;
  auto weight_i = in_tensors_.at(i_index);
  auto weight_i_data = reinterpret_cast<float *>(weight_i->data());
  CHECK_NULL_RETURN(weight_i_data);
  PackLstmWeight(weight_i_ptr_, weight_i_data, weight_batch_, lstm_param_->input_size_, lstm_param_->hidden_size_,
                 lstm_param_->input_col_align_, weights_order);

  // input bias
  input_bias_ = reinterpret_cast<float *>(malloc(weight_batch_ * lstm_param_->input_col_align_ * sizeof(float)));
  if (input_bias_ == nullptr) {
    MS_LOG(ERROR) << "LstmCPUKernel malloc input_bias_ error.";
    return RET_ERROR;
  }
  memset(input_bias_, 0, weight_batch_ * lstm_param_->input_col_align_ * sizeof(float));
  float *bias_data =
    weight_i_data + gate_num * lstm_param_->hidden_size_ * (lstm_param_->input_size_ + lstm_param_->hidden_size_);
  if (in_tensors_.size() > mindir_input_tensors) {
    bias_data = reinterpret_cast<float *>(in_tensors_.at(onnx_bias_index)->data());
  }

  CHECK_NULL_RETURN(bias_data);
  PackLstmBias(input_bias_, bias_data, weight_batch_, lstm_param_->hidden_size_, lstm_param_->input_col_align_,
               lstm_param_->bidirectional_, weights_order);
  return RET_OK;
}

int LstmCPUKernel::InitStateWeightBias() {
  // malloc and init state * weight right matrix buffer, state * weight will be executed seq_len_ times.
  // state -- row: batch; col: hidden_size
  // weight -- row: hidden_size; col: hidden_size, need transpose
  // result -- row: batch; col: hidden_size
  int weight_i_size = gate_num * lstm_param_->hidden_size_ * lstm_param_->input_size_;
  // int weight_h_size = gate_num * lstm_param_->hidden_size_ * lstm_param_->hidden_size_;
  int h_index = (in_tensors_.size() == mindir_input_tensors) ? combined_weights_index : onnx_weight_h_index;
  auto weight_h = in_tensors_.at(h_index);
  auto weight_h_data = (reinterpret_cast<float *>(weight_h->data()));
  if (in_tensors_.size() == mindir_input_tensors) {
    weight_h_data += weight_i_size;
  }

  CHECK_NULL_RETURN(weight_h_data);
  if (!state_is_vec_) {
    weight_h_ptr_ = reinterpret_cast<float *>(
      malloc(weight_batch_ * lstm_param_->state_col_align_ * lstm_param_->hidden_size_ * sizeof(float)));
    if (weight_h_ptr_ == nullptr) {
      MS_LOG(ERROR) << "LstmCPUKernel malloc weight_h_ptr_ error.";
      return RET_ERROR;
    }
    const int *weights_order = (in_tensors_.size() == mindir_input_tensors) ? weights_order_IFOG : nullptr;
    PackLstmWeight(weight_h_ptr_, weight_h_data, weight_batch_, lstm_param_->hidden_size_, lstm_param_->hidden_size_,
                   lstm_param_->state_col_align_, weights_order);
  } else {
#ifdef ENABLE_AVX
    weight_h_ptr_ = reinterpret_cast<float *>(
      malloc(weight_batch_ * lstm_param_->state_col_align_ * lstm_param_->hidden_size_ * sizeof(float)));
    if (weight_h_ptr_ == nullptr) {
      MS_LOG(ERROR) << "LstmCPUKernel malloc weight_h_ptr_ error.";
      return RET_ERROR;
    }
    for (int i = 0; i < weight_batch_; i++) {
      const float *src_batch = weight_h_data + i * lstm_param_->hidden_size_ * lstm_param_->hidden_size_;
      float *dst_batch = weight_h_ptr_ + i * lstm_param_->state_col_align_ * lstm_param_->hidden_size_;
      RowMajor2Col32Major(src_batch, dst_batch, lstm_param_->hidden_size_, lstm_param_->hidden_size_);
    }
#else
    weight_h_ptr_ = weight_h_data;
#endif
  }

  // state bias
  state_bias_ = reinterpret_cast<float *>(malloc(weight_batch_ * lstm_param_->state_col_align_ * sizeof(float)));
  if (state_bias_ == nullptr) {
    MS_LOG(ERROR) << "LstmCPUKernel malloc state_bias_ error.";
    return RET_ERROR;
  }
  memset(state_bias_, 0, weight_batch_ * lstm_param_->state_col_align_ * sizeof(float));
  // if ONNX, secend bias is also present
  if (in_tensors_.size() > mindir_input_tensors) {
    float *state_bias =
      reinterpret_cast<float *>(in_tensors_.at(onnx_bias_index)->data()) + gate_num * lstm_param_->hidden_size_;
    CHECK_NULL_RETURN(state_bias);
    PackLstmBias(state_bias_, state_bias, weight_batch_, lstm_param_->hidden_size_, lstm_param_->state_col_align_,
                 lstm_param_->bidirectional_, nullptr);
  }
  return RET_OK;
}

int LstmCPUKernel::InitParam() {
  auto input = in_tensors_.front();
  std::vector<int> in_shape = input->shape();
  lstm_param_->seq_len_ = in_shape.at(FIRST_INPUT);
  lstm_param_->batch_ = in_shape.at(SECOND_INPUT);
  lstm_param_->input_size_ = in_shape.at(THIRD_INPUT);

  auto weight_i = in_tensors_.at(onnx_weight_i_index);
  std::vector<int> w_shape = weight_i->shape();
  if (in_tensors_.size() == mindir_input_tensors) {
    hidden_state_input_index_ = mindir_hidden_state_input_index;
    cell_state_input_index_ = mindir_cell_state_input_index;
    lstm_param_->hidden_size_ = w_shape.at(THIRD_INPUT);
  } else {
    lstm_param_->hidden_size_ = w_shape.at(SECOND_INPUT) / gate_num;
  }

  lstm_param_->output_step_ = lstm_param_->bidirectional_ ? 2 * lstm_param_->batch_ * lstm_param_->hidden_size_
                                                          : lstm_param_->batch_ * lstm_param_->hidden_size_;
  weight_batch_ = lstm_param_->bidirectional_ ? 2 * gate_num : gate_num;
  state_is_vec_ = lstm_param_->batch_ == 1;

#ifdef ENABLE_AVX
  row_tile_ = C6NUM;
  col_tile_ = C16NUM;
#elif defined(ENABLE_ARM32)
  row_tile_ = C12NUM;
  col_tile_ = C4NUM;
#elif defined(ENABLE_SSE)
  row_tile_ = C4NUM;
  col_tile_ = C8NUM;
#else
  row_tile_ = C12NUM;
  col_tile_ = C8NUM;
#endif
  lstm_param_->input_row_align_ = UP_ROUND(lstm_param_->seq_len_ * lstm_param_->batch_, row_tile_);
  lstm_param_->input_col_align_ = UP_ROUND(lstm_param_->hidden_size_, col_tile_);
  input_thread_count_ = MSMIN(op_parameter_->thread_num_, UP_DIV(lstm_param_->input_col_align_, col_tile_));
  MS_CHECK_FALSE(input_thread_count_ == 0, RET_ERROR);
  input_thread_stride_ = UP_DIV(UP_DIV(lstm_param_->input_col_align_, col_tile_), input_thread_count_);

  state_row_tile_ = row_tile_;
  state_col_tile_ = col_tile_;
#ifdef ENABLE_AVX
  if (state_is_vec_) {
    state_row_tile_ = 1;
    state_col_tile_ = C8NUM;
  }
#endif

  lstm_param_->state_row_align_ = state_is_vec_ ? 1 : UP_ROUND(lstm_param_->batch_, state_row_tile_);
#ifdef ENABLE_AVX
  lstm_param_->state_col_align_ = UP_ROUND(lstm_param_->hidden_size_, state_col_tile_);
#else
  lstm_param_->state_col_align_ =
    state_is_vec_ ? lstm_param_->hidden_size_ : UP_ROUND(lstm_param_->hidden_size_, state_col_tile_);
#endif
  return RET_OK;
}

int LstmCPUKernel::Prepare() {
  CHECK_LESS_RETURN(in_tensors_.size(), mindir_input_tensors);
  for (size_t i = 0; i < in_tensors_.size(); i++) {
    CHECK_NULL_RETURN(in_tensors_.at(i));
  }
  CHECK_LESS_RETURN(out_tensors_.size(), DIMENSION_3D);
  for (size_t i = 0; i < out_tensors_.size(); i++) {
    CHECK_NULL_RETURN(out_tensors_.at(i));
  }
  CHECK_NULL_RETURN(lstm_param_);
  if (!InferShapeDone()) {
    return RET_OK;
  }
  return ReSize();
}

int LstmCPUKernel::ReSize() {
  auto ret = InitParam();
  if (ret != RET_OK) {
    MS_LOG(ERROR) << "LstmCPUKernel InitParam error.";
    return RET_ERROR;
  }

  FreeTmpBuffer();
  ret = InitInputWeightBias();
  if (ret != RET_OK) {
    MS_LOG(ERROR) << "LstmCPUKernel InitInputWeightBias error.";
    FreeTmpBuffer();
    return RET_ERROR;
  }

  ret = InitStateWeightBias();
  if (ret != RET_OK) {
    MS_LOG(ERROR) << "LstmCPUKernel InitStateWeightBias error.";
    FreeTmpBuffer();
    return RET_ERROR;
  }
  return RET_OK;
}

int LstmCPUKernel::MallocRunBuffer() {
  for (int i = 0; i < DIMENSION_7D; i++) {
    buffer_[i] = nullptr;
  }
  buffer_[packed_input_index] = reinterpret_cast<float *>(
    ms_context_->allocator->Malloc(lstm_param_->input_row_align_ * lstm_param_->input_size_ * sizeof(float)));
  if (buffer_[packed_input_index] == nullptr) {
    MS_LOG(ERROR) << "LstmCPUKernel malloc input * weight left matirx error.";
    return RET_ERROR;
  }

  buffer_[input_gate_index] = reinterpret_cast<float *>(ms_context_->allocator->Malloc(
    gate_num * lstm_param_->seq_len_ * lstm_param_->batch_ * lstm_param_->hidden_size_ * sizeof(float)));
  if (buffer_[input_gate_index] == nullptr) {
    MS_LOG(ERROR) << "LstmCPUKernel malloc input * weight result matirx error.";
    return RET_ERROR;
  }

  if (!state_is_vec_) {
    buffer_[packed_state_index] = reinterpret_cast<float *>(
      ms_context_->allocator->Malloc(lstm_param_->state_row_align_ * lstm_param_->hidden_size_ * sizeof(float)));
    if (buffer_[packed_state_index] == nullptr) {
      MS_LOG(ERROR) << "LstmCPUKernel malloc state * weight left matirx error.";
      return RET_ERROR;
    }
  }

  buffer_[state_gate_index] = reinterpret_cast<float *>(
    ms_context_->allocator->Malloc(gate_num * lstm_param_->batch_ * lstm_param_->hidden_size_ * sizeof(float)));
  if (buffer_[state_gate_index] == nullptr) {
    MS_LOG(ERROR) << "LstmCPUKernel malloc state gate buffer error.";
    return RET_ERROR;
  }

  if (!(lstm_param_->zoneout_cell_ >= -FLT_EPSILON && lstm_param_->zoneout_cell_ <= FLT_EPSILON)) {
    auto buffer_size = lstm_param_->batch_ * lstm_param_->hidden_size_ * sizeof(float);
    buffer_[cell_state_index] = reinterpret_cast<float *>(ms_context_->allocator->Malloc(buffer_size));
    if (buffer_[cell_state_index] == nullptr) {
      MS_LOG(ERROR) << "LstmCPUKernel malloc state_buffer for cell error.";
      return RET_ERROR;
    }
  }
  if (!(lstm_param_->zoneout_hidden_ >= -FLT_EPSILON && lstm_param_->zoneout_hidden_ <= FLT_EPSILON)) {
    auto buffer_size = lstm_param_->batch_ * lstm_param_->hidden_size_ * sizeof(float);
    buffer_[hidden_state_index] = reinterpret_cast<float *>(ms_context_->allocator->Malloc(buffer_size));
    if (buffer_[hidden_state_index] == nullptr) {
      MS_LOG(ERROR) << "LstmCPUKernel malloc state_buffer for hidden error.";
      return RET_ERROR;
    }
  }
#ifdef ENABLE_AVX
  if (state_is_vec_) {  // vec matmul need to malloc dst
    output_need_packed_ = lstm_param_->hidden_size_ % state_col_tile_;
    if (output_need_packed_) {
      int out_channel = lstm_param_->hidden_size_;
      int oc_block_num = UP_DIV(out_channel, state_col_tile_);
      MS_ASSERT(ms_context_->allocator != nullptr);
      buffer_[avx_state_output_index] = reinterpret_cast<float *>(
        ms_context_->allocator->Malloc(lstm_param_->batch_ * oc_block_num * state_col_tile_ * sizeof(float)));
      if (buffer_[avx_state_output_index] == nullptr) {
        MS_LOG(ERROR) << "LstmCPUKernel malloc tmp output data failed.";
        return RET_ERROR;
      }
    }
  }
#endif
  return RET_OK;
}

void LstmCPUKernel::InputWeightMatMul(int task_id) const {
  int current_start_oc = task_id * input_thread_stride_ * col_tile_;
  int current_rest_oc = 0;
  current_rest_oc = lstm_param_->hidden_size_ - current_start_oc;
  int cur_oc = MSMIN(input_thread_stride_ * col_tile_, current_rest_oc);
  if (cur_oc <= 0) {
    return;
  }

  auto input = buffer_[packed_input_index];
  auto b = weight_loop_ + current_start_oc * lstm_param_->input_size_;
  auto c = gate_loop_ + current_start_oc;
  auto bias = (bias_loop_ == nullptr) ? nullptr : bias_loop_ + current_start_oc;
  MatMulOpt(input, b, c, bias, ActType_No, lstm_param_->input_size_, lstm_param_->seq_len_ * lstm_param_->batch_,
            cur_oc, lstm_param_->hidden_size_, OutType_Nhwc);
}

int LstmInputMulWeightRun(const void *cdata, int task_id, float, float) {
  auto kernel = reinterpret_cast<const LstmCPUKernel *>(cdata);
  CHECK_NULL_RETURN(kernel);
  kernel->InputWeightMatMul(task_id);
  return RET_OK;
}

int LstmCPUKernel::LstmUnidirectional(float *output, const float *weight_i, const float *weight_h,
                                      const float *input_bias, const float *state_bias, float *hidden_state,
                                      float *cell_state, bool is_backward) {
  float *gate = buffer_[input_gate_index];
  for (int i = 0; i < gate_num; i++) {
    weight_loop_ = weight_i + lstm_param_->input_size_ * lstm_param_->input_col_align_ * i;
    bias_loop_ = input_bias + lstm_param_->input_col_align_ * i;
    gate_loop_ = gate + lstm_param_->seq_len_ * lstm_param_->batch_ * lstm_param_->hidden_size_ * i;
    auto ret = ParallelLaunch(this->ms_context_, LstmInputMulWeightRun, this, input_thread_count_);
    if (ret != RET_OK) {
      return RET_ERROR;
    }
  }

  float *input_gate = gate;
  float *forget_gate = gate + lstm_param_->seq_len_ * lstm_param_->batch_ * lstm_param_->hidden_size_ * 2;
  float *cell_gate = gate + lstm_param_->seq_len_ * lstm_param_->batch_ * lstm_param_->hidden_size_ * 3;
  float *output_gate = gate + lstm_param_->seq_len_ * lstm_param_->batch_ * lstm_param_->hidden_size_;
  for (int t = 0; t < lstm_param_->seq_len_; t++) {
    int real_t = is_backward ? lstm_param_->seq_len_ - t - 1 : t;
    float *input_gate_t = input_gate + lstm_param_->batch_ * lstm_param_->hidden_size_ * real_t;
    float *forget_gate_t = forget_gate + lstm_param_->batch_ * lstm_param_->hidden_size_ * real_t;
    float *cell_gate_t = cell_gate + lstm_param_->batch_ * lstm_param_->hidden_size_ * real_t;
    float *output_gate_t = output_gate + lstm_param_->batch_ * lstm_param_->hidden_size_ * real_t;
    float *output_ptr = output + real_t * lstm_param_->output_step_;

    if (IsTrain() && IsTrainable()) {
      RecordPreState(cell_state, is_backward ? real_t : t);
    }
    LstmStepUnit(output_ptr, input_gate_t, forget_gate_t, cell_gate_t, output_gate_t, weight_h, state_bias,
                 hidden_state, cell_state, buffer_, lstm_param_);
    if (IsTrain() && IsTrainable()) {
      RecordStates(hidden_state, cell_state, input_gate_t, output_gate_t, forget_gate_t, cell_gate_t,
                   is_backward ? real_t : t);
    }
  }
  return RET_OK;
}

void LstmCPUKernel::RecordPreState(float *cell_state_minus1, int step) {
  float *states = reinterpret_cast<float *>(out_tensors_[out_intermediate_states_index]->data());
  auto state_size = lstm_param_->batch_ * lstm_param_->hidden_size_;
  auto stride = step * state_size;
  auto seq_stride = (lstm_param_->seq_len_ + 1) * state_size;
  std::cout << "RECORD pre state. step: " << step << std::endl;
  std::cout << "batch: " << lstm_param_->batch_ << "hidden_size" << lstm_param_->hidden_size_
            << " seq_len_: " << lstm_param_->seq_len_ << std::endl;
  std::cout << "state_size: " << state_size << " stride: " << stride << " seq_stride " << seq_stride << std::endl;
  for (int ix = 0; ix < state_size; ix++) {
    std::cout << "index: " << ix << " CS_minus1: " << cell_state_minus1[ix] << std::endl;
  }
  stride += (no_of_recorde_values - 1) * seq_stride;
  memcpy(states + stride, cell_state_minus1, state_size * sizeof(float));
}

void LstmCPUKernel::RecordStates(float *hidden_state, float *cell_state, float *input_gate, float *output_gate,
                                 float *forget_gate, float *cell_gate, int step) {
  float *states = reinterpret_cast<float *>(out_tensors_[out_intermediate_states_index]->data());
  auto state_size = lstm_param_->batch_ * lstm_param_->hidden_size_;
  auto stride = step * state_size;
  auto seq_stride = lstm_param_->seq_len_ * state_size;
  std::cout << "RECORD step: " << step << std::endl;
  std::cout << "batch: " << lstm_param_->batch_ << "hidden_size" << lstm_param_->hidden_size_
            << " seq_len_: " << lstm_param_->seq_len_ << std::endl;
  std::cout << "state_size: " << state_size << " stride: " << stride << " seq_stride " << seq_stride << std::endl;
  for (int ix = 0; ix < state_size; ix++) {
    std::cout << "index: " << ix << " HS: " << hidden_state[ix] << " CS: " << cell_state[ix]
              << " IG: " << input_gate[ix] << " OG: " << output_gate[ix] << " FG: " << forget_gate[ix]
              << " CG: " << cell_gate[ix] << std::endl;
  }

  memcpy(states + stride, hidden_state, state_size * sizeof(float));
  stride += seq_stride;
  memcpy(states + stride, cell_state, state_size * sizeof(float));
  stride += seq_stride;
  memcpy(states + stride, input_gate, state_size * sizeof(float));
  stride += seq_stride;
  memcpy(states + stride, output_gate, state_size * sizeof(float));
  stride += seq_stride;
  memcpy(states + stride, forget_gate, state_size * sizeof(float));
  stride += seq_stride;
  memcpy(states + stride, cell_gate, state_size * sizeof(float));
}

int LstmCPUKernel::InnerExecute(float *output, const float *input, float *hidden_state, float *cell_state) {
  // forward
  // buffer_[packed_input_index] : store packed input
  PackLstmInput(input, buffer_[packed_input_index], lstm_param_->seq_len_ * lstm_param_->batch_,
                lstm_param_->input_size_);
  auto ret =
    LstmUnidirectional(output, weight_i_ptr_, weight_h_ptr_, input_bias_, state_bias_, hidden_state, cell_state, false);
  if (ret != RET_OK) {
    MS_LOG(ERROR) << "Lstm unidirectional calculation error.";
    return RET_ERROR;
  }

  // backward
  if (lstm_param_->bidirectional_) {
    const float *backward_weight_i =
      weight_i_ptr_ + gate_num * lstm_param_->input_col_align_ * lstm_param_->input_size_;
    const float *backward_weight_h =
      weight_h_ptr_ + gate_num * lstm_param_->state_col_align_ * lstm_param_->hidden_size_;
    const float *backward_input_bias = input_bias_ + gate_num * lstm_param_->input_col_align_;
    const float *backward_state_bias = state_bias_ + gate_num * lstm_param_->state_col_align_;
    float *backward_output = output + lstm_param_->batch_ * lstm_param_->hidden_size_;
    float *backward_cell_state = cell_state + lstm_param_->batch_ * lstm_param_->hidden_size_;
    float *backward_hidden_state = hidden_state + lstm_param_->batch_ * lstm_param_->hidden_size_;

    ret = LstmUnidirectional(backward_output, backward_weight_i, backward_weight_h, backward_input_bias,
                             backward_state_bias, backward_hidden_state, backward_cell_state, true);
    if (ret != RET_OK) {
      MS_LOG(ERROR) << "Lstm bidirectional calculation error.";
      return RET_ERROR;
    }
  }
  return RET_OK;
}

int LstmCPUKernel::Run() {
  auto input = in_tensors_.at(0);
  auto output = out_tensors_.at(0);
  auto input_ptr = reinterpret_cast<float *>(input->data());
  CHECK_NULL_RETURN(input_ptr);
  auto output_ptr = reinterpret_cast<float *>(output->data());
  CHECK_NULL_RETURN(output_ptr);

  auto hidden_state = in_tensors_.at(hidden_state_input_index_);
  CHECK_NULL_RETURN(hidden_state->data());
  auto cell_state = in_tensors_.at(cell_state_input_index_);
  CHECK_NULL_RETURN(cell_state->data());

  auto output_hidden_state = out_tensors_[1];
  CHECK_NULL_RETURN(output_hidden_state->data());
  memcpy(output_hidden_state->data(), hidden_state->data(), hidden_state->ElementsNum() * sizeof(float));
  auto output_cell_state = out_tensors_[2];
  CHECK_NULL_RETURN(output_cell_state->data());
  memcpy(output_cell_state->data(), cell_state->data(), cell_state->ElementsNum() * sizeof(float));

  auto ret = MallocRunBuffer();
  if (ret != RET_OK) {
    MS_LOG(ERROR) << "LstmCPUKernel MallocRunBuffer error.";
    FreeRunBuffer();
    return RET_ERROR;
  }

  CHECK_NULL_RETURN(weight_h_ptr_);
  CHECK_NULL_RETURN(weight_i_ptr_);
  CHECK_NULL_RETURN(input_bias_);
  CHECK_NULL_RETURN(state_bias_);
  ret = InnerExecute(output_ptr, input_ptr, reinterpret_cast<float *>(output_hidden_state->data()),
                     reinterpret_cast<float *>(output_cell_state->data()));
  FreeRunBuffer();
  return ret;
}

REG_KERNEL(kCPU, kNumberTypeFloat32, PrimitiveType_LSTM, LiteKernelCreator<LstmCPUKernel>)
}  // namespace luojianet_ms::kernel
