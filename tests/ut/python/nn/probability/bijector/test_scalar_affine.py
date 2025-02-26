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
"""test cases for scalar affine"""
import pytest
import luojianet_ms.nn as nn
import luojianet_ms.nn.probability.bijector as msb
from luojianet_ms import Tensor
from luojianet_ms import dtype
from luojianet_ms import context

skip_flag = context.get_context("device_target") == "CPU"


def test_init():
    """
    Test initializations.
    """
    b = msb.ScalarAffine()
    assert isinstance(b, msb.Bijector)
    b = msb.ScalarAffine(scale=1.0)
    assert isinstance(b, msb.Bijector)
    b = msb.ScalarAffine(shift=2.0)
    assert isinstance(b, msb.Bijector)
    b = msb.ScalarAffine(3.0, 4.0)
    assert isinstance(b, msb.Bijector)


def test_type():
    with pytest.raises(TypeError):
        msb.ScalarAffine(scale='scale')
    with pytest.raises(TypeError):
        msb.ScalarAffine(shift='shift')
    with pytest.raises(TypeError):
        msb.ScalarAffine(name=0.1)


class ForwardBackward(nn.Module):
    """
    Test class: forward and backward pass.
    """
    def __init__(self):
        super(ForwardBackward, self).__init__()
        self.b1 = msb.ScalarAffine(2.0, 1.0)
        self.b2 = msb.ScalarAffine()

    def call(self, x_):
        ans1 = self.b1.inverse(self.b1.forward(x_))
        ans2 = self.b2.inverse(self.b2.forward(x_))
        return ans1 + ans2


@pytest.mark.skipif(skip_flag, reason="not support running in CPU")
def test_forward_and_backward_pass():
    """
    Test forward and backward pass of ScalarAffine bijector.
    """
    net = ForwardBackward()
    x = Tensor([2.0, 3.0, 4.0, 5.0], dtype=dtype.float32)
    ans = net(x)
    assert isinstance(ans, Tensor)


class ForwardJacobian(nn.Module):
    """
    Test class: Forward log Jacobian.
    """
    def __init__(self):
        super(ForwardJacobian, self).__init__()
        self.b1 = msb.ScalarAffine(2.0, 1.0)
        self.b2 = msb.ScalarAffine()

    def call(self, x_):
        ans1 = self.b1.forward_log_jacobian(x_)
        ans2 = self.b2.forward_log_jacobian(x_)
        return ans1 + ans2


@pytest.mark.skipif(skip_flag, reason="not support running in CPU")
def test_forward_jacobian():
    """
    Test forward log jacobian of ScalarAffine bijector.
    """
    net = ForwardJacobian()
    x = Tensor([2.0, 3.0, 4.0, 5.0], dtype=dtype.float32)
    ans = net(x)
    assert isinstance(ans, Tensor)


class BackwardJacobian(nn.Module):
    """
    Test class: Backward log Jacobian.
    """
    def __init__(self):
        super(BackwardJacobian, self).__init__()
        self.b1 = msb.ScalarAffine(2.0, 1.0)
        self.b2 = msb.ScalarAffine()

    def call(self, x_):
        ans1 = self.b1.inverse_log_jacobian(x_)
        ans2 = self.b2.inverse_log_jacobian(x_)
        return ans1 + ans2


@pytest.mark.skipif(skip_flag, reason="not support running in CPU")
def test_backward_jacobian():
    """
    Test backward log jacobian of ScalarAffine bijector.
    """
    net = BackwardJacobian()
    x = Tensor([2.0, 3.0, 4.0, 5.0], dtype=dtype.float32)
    ans = net(x)
    assert isinstance(ans, Tensor)


class Net(nn.Module):
    """
    Test class: function calls going through call.
    """
    def __init__(self):
        super(Net, self).__init__()
        self.b1 = msb.ScalarAffine(1.0, 0.0)
        self.b2 = msb.ScalarAffine()

    def call(self, x_):
        ans1 = self.b1('inverse', self.b1('forward', x_))
        ans2 = self.b2('inverse', self.b2('forward', x_))
        ans3 = self.b1('forward_log_jacobian', x_)
        ans4 = self.b2('forward_log_jacobian', x_)
        ans5 = self.b1('inverse_log_jacobian', x_)
        ans6 = self.b2('inverse_log_jacobian', x_)
        return ans1 - ans2 + ans3 -ans4 + ans5 - ans6


@pytest.mark.skipif(skip_flag, reason="not support running in CPU")
def test_old_api():
    """
    Test old api which goes through call.
    """
    net = Net()
    x = Tensor([2.0, 3.0, 4.0, 5.0], dtype=dtype.float32)
    ans = net(x)
    assert isinstance(ans, Tensor)
