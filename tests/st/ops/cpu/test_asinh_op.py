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

import luojianet_ms.nn as nn
from luojianet_ms import Tensor
from luojianet_ms import context
from luojianet_ms.ops import operations as P

context.set_context(mode=context.GRAPH_MODE, device_target="CPU")


class NetAsinh(nn.Module):
    def __init__(self):
        super(NetAsinh, self).__init__()
        self.asinh = P.Asinh()

    def call(self, x):
        return self.asinh(x)


@pytest.mark.level0
@pytest.mark.platform_x86_cpu
@pytest.mark.env_onecard
@pytest.mark.parametrize('dtype', [np.float32, np.float64])
def test_asinh(dtype):
    """
    Feature: ALL To ALL
    Description: test cases for Asinh
    Expectation: the result match to numpy
    """
    np_array = np.array([-1, -0.5, 0, 0.5, 1], dtype=dtype)
    input_x = Tensor(np_array)
    net = NetAsinh()
    output = net(input_x)
    print(output)
    expect = np.arcsinh(np_array)
    assert np.allclose(output.asnumpy(), expect)
