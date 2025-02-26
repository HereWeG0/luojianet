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

#ifndef LUOJIANET_MS_LITE_TOOLS_OPTIMIZER_FISSON_FISSON_UTIL_H_
#define LUOJIANET_MS_LITE_TOOLS_OPTIMIZER_FISSON_FISSON_UTIL_H_

#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include "schema/inner/model_generated.h"
#include "luojianet_ms/ccsrc/utils/utils.h"
#include "tools/optimizer/common/gllo_utils.h"
#include "tools/converter/converter_flags.h"
#include "luojianet_ms/lite/include/context.h"
#include "luojianet_ms/lite/include/lite_types.h"
#include "ops/fusion/conv2d_fusion.h"

namespace luojianet_ms {
using luojianet_ms::schema::PrimitiveType;
namespace opt {

struct SplitInfo {
  int64_t axis{0};
  int64_t out_num{0};
  std::vector<int64_t> size_splits{};
  std::vector<int64_t> extend_top{};
  std::vector<int64_t> extend_bottom{};
  std::vector<luojianet_ms::lite::DeviceType> dev_types{};
  int64_t ori_split_axis_value{0};
  int64_t in_num_conv{0};
  int64_t fmk_type{0};
  PrimitiveType primitive_type{schema::PrimitiveType_NONE};
};

typedef enum { CUT_N, CUT_H, CUT_W, CUT_C_IN, CUT_C_OUT, CUT_NONE } CuttingStragedy;

std::vector<int64_t> GetSplitPadList(const std::shared_ptr<ops::Conv2DFusion> &ori_conv_prim, int64_t input_h,
                                     int64_t input_w);

bool IsConv2D(const AnfNodePtr &node);

std::shared_ptr<ops::Conv2DFusion> CopyConvPrim(const std::shared_ptr<ops::Conv2DFusion> &ori_attr);

bool UpdateSplitInfo(const FuncGraphPtr &func_graph, const std::vector<AnfNodePtr> &conv_nodes, SplitInfo *split_info);

bool GetMultipleOutputsOfAnfNode(const FuncGraphPtr &func_graph, const AnfNodePtr &node, size_t output_num,
                                 std::vector<AnfNodePtr> *outputs);

AnfNodePtr CreateOutputsOfConcat(const FuncGraphPtr &func_graph, const AnfNodePtr &conv_cnode,
                                 const std::vector<AnfNodePtr> &conv_outputs, SplitInfo *split_info,
                                 const std::string &node_name);
bool CreateOutputsOfSplitWithOverlap(const FuncGraphPtr &func_graph, const AnfNodePtr &conv_cnode,
                                     std::vector<AnfNodePtr> *split_outputs, SplitInfo *split_info,
                                     const std::string &node_name);
bool UpdateRatioWithPadStride(int64_t *ratio, size_t ratio_len, size_t split_size, int split_dim_size);
}  // namespace opt
}  // namespace luojianet_ms
#endif  // LUOJIANET_MS_LITE_TOOLS_OPTIMIZER_FISSON_FISSON_UTIL_H_
