/**
 * Copyright 2020 Huawei Technologies Co., Ltd
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

#include "common/backend_common_test.h"
#include "common/py_func_graph_fetcher.h"
#include "backend/optimizer/ascend/ir_fission/tensor_scatter_update_fission.h"
#include "debug/anf_ir_dump.h"

namespace luojianet_ms {
namespace opt {
class TestHWOptTensorScatterUpdateFission : public BackendCommon {
 public:
  TestHWOptTensorScatterUpdateFission()
      : get_py_fun_("gtest_input.pre_activate.tensor_scatter_update_fission_test", true) {}
  ~TestHWOptTensorScatterUpdateFission() override = default;

  UT::PyFuncGraphFetcher get_py_fun_;
};

TEST_F(TestHWOptTensorScatterUpdateFission, test_fission) {
  FuncGraphPtr g = get_py_fun_.CallAndParseRet("test_tensor_scatter_update_fission", "before");
  EXPECT_NE(g, nullptr);
  std::vector<int64_t> shp1{2, 3};
  std::vector<int64_t> shp2{2, 2};
  std::vector<int64_t> shp3{2};
  auto inputx = std::make_shared<abstract::AbstractTensor>(kFloat32, shp1);
  auto indices = std::make_shared<abstract::AbstractTensor>(kInt32, shp2);
  auto update = std::make_shared<abstract::AbstractTensor>(kFloat32, shp3);
  AbstractBasePtrList args_spec_list{inputx, indices, update};
  auto fg = GetKernelGraph(g, args_spec_list);

  auto optimizer = std::make_shared<opt::GraphOptimizer>();
  auto pm = std::make_shared<opt::PassManager>();
  pm->AddPass(std::make_shared<opt::TensorScatterUpdateFission>());
  optimizer->AddPassManager(pm);
  FuncGraphPtr new_graph = optimizer->Optimize(fg);

  FuncGraphPtr g_after = get_py_fun_.CallAndParseRet("test_tensor_scatter_update_fission", "after");
  EXPECT_NE(g_after, nullptr);
  EXPECT_TRUE(CheckEqualGraph(g_after, new_graph));
}
}  // namespace opt
}  // namespace luojianet_ms
