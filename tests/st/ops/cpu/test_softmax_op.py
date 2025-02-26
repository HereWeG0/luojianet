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
from luojianet_ms.common.initializer import initializer
from luojianet_ms.common.parameter import Parameter
from luojianet_ms.ops import operations as P

context.set_context(mode=context.GRAPH_MODE, device_target='CPU')


class NetSoftmax(nn.Module):
    def __init__(self):
        super(NetSoftmax, self).__init__()
        self.softmax = P.Softmax(axis=-1)
        x = Tensor(np.array([[0.1, 0.3, 0.6],
                             [0.2, -0.6, 0.8],
                             [0.6, 1, 0.4]]).astype(np.float32))
        self.x = Parameter(initializer(x, x.shape), name='x')

    def call(self):
        return self.softmax(self.x)


@pytest.mark.level0
@pytest.mark.platform_x86_cpu
@pytest.mark.env_onecard
def test_softmax():
    Softmax = NetSoftmax()
    output = Softmax()
    output = output.asnumpy()
    outputSum = output.sum(axis=1)
    expect = np.ones(3)
    error = expect * 1.0e-6
    diff = np.abs(outputSum - expect)
    print(diff)
    assert np.all(diff < error)


class NetSoftmax1(nn.Module):
    def __init__(self):
        super(NetSoftmax1, self).__init__()
        self.softmax = P.Softmax(axis=-2)
        x = Tensor(np.array([[0.1, 0.3, 0.6],
                             [0.2, -0.6, 0.8],
                             [0.6, 1, 0.4]]).astype(np.float32))
        self.x = Parameter(initializer(x, x.shape), name='x')

    def call(self):
        return self.softmax(self.x)


@pytest.mark.level0
@pytest.mark.platform_x86_cpu
@pytest.mark.env_onecard
def test_softmax1():
    Softmax = NetSoftmax1()
    output = Softmax()
    output = output.asnumpy()
    outputSum = output.sum(axis=0)
    expect = np.ones(3)
    error = expect * 1.0e-6
    diff = np.abs(outputSum - expect)
    print(diff)
    assert np.all(diff < error)
