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
from luojianet_ms.ops.operations import _grad_ops as G


class NetSqrtGrad(nn.Module):
    def __init__(self):
        super(NetSqrtGrad, self).__init__()
        self.sqrt_grad = G.SqrtGrad()

    def call(self, x, dx):
        return self.sqrt_grad(x, dx)


@pytest.mark.level0
@pytest.mark.platform_x86_gpu_training
@pytest.mark.env_onecard
def test_sqrt_grad():
    x = Tensor(np.array([[[[-1, 1, 10],
                           [5.9, 6.1, 6],
                           [10, 1, -1]]]]).astype(np.float32))
    dx = Tensor(np.array([[[[1, 1, 1],
                            [2, 2, 2],
                            [3, 3, 3]]]]).astype(np.float32))
    expect = np.array([[[[-0.5, 0.5, 0.05,],
                         [0.16949153, 0.16393442, 0.16666667,],
                         [0.15, 1.5, -1.5,]]]]).astype(np.float32)
    error = np.ones(shape=[3, 3]) * 1.0e-6

    context.set_context(mode=context.GRAPH_MODE, device_target="GPU")
    sqrt_grad = NetSqrtGrad()
    output = sqrt_grad(x, dx)
    diff = output.asnumpy() - expect
    assert np.all(np.abs(diff) < error)
