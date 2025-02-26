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
"""test cases for exp"""
import pytest
import luojianet_ms.nn as nn
import luojianet_ms.nn.probability.bijector as msb
from luojianet_ms import Tensor
from luojianet_ms import dtype
from luojianet_ms import context

skip_flag = context.get_context("device_target") == "CPU"


def test_init():
    b = msb.Exp()
    assert isinstance(b, msb.Bijector)


def test_type():
    with pytest.raises(TypeError):
        msb.Exp(name=0.1)


class Net(nn.Module):
    """
    Test class: forward and inverse pass of bijector.
    """

    def __init__(self):
        super(Net, self).__init__()
        self.b1 = msb.Exp()
        self.b2 = msb.Exp()

    def call(self, x_):
        forward = self.b1.forward(x_)
        inverse = self.b1.inverse(forward)
        return x_ - inverse


@pytest.mark.skipif(skip_flag, reason="not support running in CPU")
def test1():
    """
    Test forward and inverse pass of exp bijector.
    """
    net = Net()
    x = Tensor([2.0, 3.0, 4.0, 5.0], dtype=dtype.float32)
    ans = net(x)
    assert isinstance(ans, Tensor)


class Jacobian(nn.Module):
    """
    Test class: forward and inverse pass of bijector.
    """

    def __init__(self):
        super(Jacobian, self).__init__()
        self.b1 = msb.Exp()
        self.b2 = msb.Exp()

    def call(self, x_):
        ans1 = self.b1.forward_log_jacobian(x_)
        ans2 = self.b1.inverse_log_jacobian(x_)
        return ans1 + ans2


@pytest.mark.skipif(skip_flag, reason="not support running in CPU")
def test2():
    """
    Test jacobians of exp bijector.
    """
    net = Jacobian()
    x = Tensor([2.0, 3.0, 4.0, 5.0], dtype=dtype.float32)
    ans = net(x)
    assert isinstance(ans, Tensor)
