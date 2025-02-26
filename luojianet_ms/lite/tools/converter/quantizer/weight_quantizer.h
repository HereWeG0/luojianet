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

#ifndef LUOJIANET_MS_LITE_TOOLS_CONVERTER_QUANTIZER_WEIGHT_QUANTIZER_H_
#define LUOJIANET_MS_LITE_TOOLS_CONVERTER_QUANTIZER_WEIGHT_QUANTIZER_H_

#include <future>
#include <memory>
#include <unordered_map>
#include <map>
#include <list>
#include <string>
#include <utility>
#include <vector>
#include <set>
#include "tools/converter/quantizer/quantizer.h"
#include "tools/converter/quantizer/quantize_util.h"
#include "tools/converter/quantizer/quant_params.h"
#include "tools/converter/quantizer/quant_strategy.h"
#include "tools/converter/preprocess/preprocess_param.h"
#include "ir/func_graph.h"
#include "ir/anf.h"
#include "include/model.h"
#include "base/base.h"
#include "abstract/dshape.h"
#include "src/lite_session.h"
#include "src/common/quant_utils.h"

namespace luojianet_ms::lite::quant {
class WeightQuantizer : public Quantizer {
 public:
  explicit WeightQuantizer(const converter::Flags &flags) : Quantizer(flags) {
    bit_num_ = flags.commonQuantParam.bit_num;
    if (bit_num_ == 0) {
      type_id_ = kNumberTypeInt16;
      is_mixed_bit_ = true;
      mixed_bit_init_scale_ = flags.mixedBitWeightQuantParam.init_scale;
    }
    // parse param for fixed bit quant.
    if (!is_mixed_bit_) {
      quant_max_ = QuantMax(bit_num_, false);
      quant_min_ = QuantMin(bit_num_, false, false);
      // parse type_id_
      MS_ASSERT(bit_num_ > 0 && bit_num_ <= k16Bit);
      if (bit_num_ > 0 && bit_num_ <= k8Bit) {
        type_id_ = kNumberTypeInt8;
      } else if (bit_num_ <= k16Bit) {
        type_id_ = kNumberTypeInt16;
      }
    }
  }
  ~WeightQuantizer() override;

  int DoQuantize(FuncGraphPtr func_graph) override;
  int DoQuantize(const FuncGraphPtr &func_graph, double init_scale);

 private:
  int DoWeightQuantize(const FuncGraphPtr &func_graph, const CNodePtr &cnode);
  int MarkWeightQuantizationInNodes(const FuncGraphPtr &);
  int DoMarkWeightQuantizeIfQuantized(const CNodePtr &);

 private:
  size_t bit_num_{8};
  // delete it in the future.
  std::set<tensor::TensorPtr> weight_quantized_tensors_;
  std::vector<std::unordered_map<std::string, luojianet_ms::tensor::MSTensor *>> fp32_output_tensors_;
  bool is_mixed_bit_ = false;
  double mixed_bit_init_scale_ = 0.02;

  int quant_max_{127};
  int quant_min_{-128};
  TypeId type_id_{kNumberTypeInt8};
};
}  // namespace luojianet_ms::lite::quant
#endif  // LUOJIANET_MS_LITE_TOOLS_CONVERTER_QUANTIZER_WEIGHT_QUANTIZER_H_
