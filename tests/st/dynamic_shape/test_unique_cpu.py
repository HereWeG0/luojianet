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
import luojianet_ms.common.dtype as mstype
from luojianet_ms.ops import operations as P

context.set_context(mode=context.GRAPH_MODE, device_target="CPU")


class Net(nn.Module):
    def __init__(self):
        super(Net, self).__init__()
        self.unique = P.Unique()

    def call(self, x):
        return self.unique(x)


class UniqueSquare(nn.Module):
    def __init__(self):
        super(UniqueSquare, self).__init__()
        self.unique = P.Unique()
        self.square = P.Square()

    def call(self, x):
        x, _ = self.unique(x)
        return self.square(x)


@pytest.mark.level0
@pytest.mark.platform_arm_ascend_training
@pytest.mark.platform_x86_ascend_training
@pytest.mark.env_onecard
def test_unique_cpu():
    x = Tensor(np.array([1, 1, 2, 2, 3, 3]), mstype.int32)
    unique = Net()
    output = unique(x)
    expect1 = np.array([1, 2, 3])
    expect2 = np.array([0, 0, 1, 1, 2, 2])
    assert (output[0].asnumpy() == expect1).all()
    assert (output[1].asnumpy() == expect2).all()


@pytest.mark.level0
@pytest.mark.platform_arm_ascend_training
@pytest.mark.platform_x86_ascend_training
@pytest.mark.env_onecard
def test_unique_square():
    x = Tensor(np.array([1, 1, 2, 2, 3, 3]), mstype.int32)
    net = UniqueSquare()
    output = net(x)
    expect1 = np.array([1, 4, 9])
    assert (output.asnumpy() == expect1).all()
