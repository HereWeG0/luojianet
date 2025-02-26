# Copyright 2021, 2022 LuoJiaNET Research and Development Group, Wuhan University
# Copyright 2021, 2022 Huawei Technologies Co., Ltd
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# ============================================================================

import numpy as np
import pytest
import luojianet_ms.context as context
import luojianet_ms.nn as nn
from luojianet_ms import Tensor
from luojianet_ms.common.parameter import Parameter
from luojianet_ms.ops import operations as P
import luojianet_ms.common.dtype as mstype


class Net(nn.Module):
    def __init__(self):
        super(Net, self).__init__()
        self.sparse_apply_proximal_adagrad = P.FusedSparseProximalAdagrad()
        self.var = Parameter(Tensor(np.ones([3, 3, 3]).astype(np.float32)), name="var")
        self.accum = Parameter(Tensor(np.ones([3, 3, 3]).astype(np.float32)), name="accum")
        self.lr = 0.01
        self.l1 = 0.0
        self.l2 = 0.0

    def call(self, grad, indices):
        out = self.sparse_apply_proximal_adagrad(self.var, self.accum, self.lr, self.l1, self.l2, grad, indices)
        return out


@pytest.mark.level0
@pytest.mark.platform_x86_cpu
@pytest.mark.env_onecard
def test_net():
    gradient = Tensor(np.ones([3, 3, 3]).astype(np.float32))
    indices = Tensor([0, 1, 2], mstype.int32)

    context.set_context(mode=context.GRAPH_MODE, device_target="CPU")
    sparse_apply_proximal_adagrad = Net()
    sparse_apply_proximal_adagrad(gradient, indices)
    print(sparse_apply_proximal_adagrad.var.data)
    expect_var = np.array([[[0.9929289, 0.9929289, 0.9929289],
                            [0.9929289, 0.9929289, 0.9929289],
                            [0.9929289, 0.9929289, 0.9929289]],
                           [[0.9929289, 0.9929289, 0.9929289],
                            [0.9929289, 0.9929289, 0.9929289],
                            [0.9929289, 0.9929289, 0.9929289]],
                           [[0.9929289, 0.9929289, 0.9929289],
                            [0.9929289, 0.9929289, 0.9929289],
                            [0.9929289, 0.9929289, 0.9929289]]]).astype(np.float32)
    assert np.all(sparse_apply_proximal_adagrad.var.data.asnumpy() == expect_var)
