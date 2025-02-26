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
import numpy as np
import luojianet_ms.context as context
import luojianet_ms.nn as nn
import luojianet_ms.nn.probability.bijector as msb
from luojianet_ms import Tensor
from luojianet_ms import dtype

context.set_context(mode=context.GRAPH_MODE, device_target="Ascend")

class Net(nn.Module):
    """
    Test class: forward pass of bijector.
    """
    def __init__(self):
        super(Net, self).__init__()
        self.bijector = msb.ScalarAffine(scale=2.0, shift=1.0)

    def call(self, x_):
        return self.bijector.forward(x_)

def test_forward():
    forward = Net()
    x = np.array([2.0, 3.0, 4.0, 5.0]).astype(np.float32)
    ans = forward(Tensor(x, dtype=dtype.float32))
    tol = 1e-6
    expected = 2 * x + 1
    assert (np.abs(ans.asnumpy() - expected) < tol).all()

class Net1(nn.Module):
    """
    Test class: backward pass of bijector.
    """
    def __init__(self):
        super(Net1, self).__init__()
        self.bijector = msb.ScalarAffine(shift=1.0, scale=2.0)

    def call(self, x_):
        return self.bijector.inverse(x_)

def test_backward():
    backward = Net1()
    x = np.array([2.0, 3.0, 4.0, 5.0]).astype(np.float32)
    ans = backward(Tensor(x, dtype=dtype.float32))
    tol = 1e-6
    expected = 0.5 * (x - 1.0)
    assert (np.abs(ans.asnumpy() - expected) < tol).all()

class Net2(nn.Module):
    """
    Test class: Forward Jacobian.
    """
    def __init__(self):
        super(Net2, self).__init__()
        self.bijector = msb.ScalarAffine(shift=1.0, scale=2.0)

    def call(self, x_):
        return self.bijector.forward_log_jacobian(x_)

def test_forward_jacobian():
    forward_jacobian = Net2()
    x = Tensor([2.0, 3.0, 4.0, 5.0], dtype=dtype.float32)
    ans = forward_jacobian(x)
    expected = np.log([2.0])
    tol = 1e-6
    assert (np.abs(ans.asnumpy() - expected) < tol).all()

class Net3(nn.Module):
    """
    Test class: Backward Jacobian.
    """
    def __init__(self):
        super(Net3, self).__init__()
        self.bijector = msb.ScalarAffine(shift=1.0, scale=2.0)

    def call(self, x_):
        return self.bijector.inverse_log_jacobian(x_)

def test_backward_jacobian():
    backward_jacobian = Net3()
    x = Tensor([2.0, 3.0, 4.0, 5.0], dtype=dtype.float32)
    ans = backward_jacobian(x)
    expected = np.log([0.5])
    tol = 1e-6
    assert (np.abs(ans.asnumpy() - expected) < tol).all()
