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
"""test vjp in pynative mode"""
import numpy as np
import pytest
import luojianet_ms.nn as nn
import luojianet_ms.context as context
from luojianet_ms import Tensor
from luojianet_ms import ms_function
from luojianet_ms.ops.functional import vjp

context.set_context(mode=context.PYNATIVE_MODE)


class SingleInputNet(nn.Module):
    def call(self, x):
        return x**3


class MultipleInputsOutputNet(nn.Module):
    def call(self, x, y):
        return 2*x, y**3


@pytest.mark.level0
@pytest.mark.platform_x86_cpu
@pytest.mark.env_onecard
def test_vjp_single_input_graph():
    x = Tensor(np.array([[1, 2], [3, 4]]).astype(np.float32))
    v = Tensor(np.array([[1, 1], [1, 1]]).astype(np.float32))
    net = SingleInputNet()
    expect_primal = Tensor(np.array([[1, 8], [27, 64]]).astype(np.float32))
    expect_grad = Tensor(np.array([[3, 12], [27, 48]]).astype(np.float32))
    primal, grad = vjp(net, x, v)
    assert np.allclose(primal.asnumpy(), expect_primal.asnumpy())
    assert np.allclose(grad.asnumpy(), expect_grad.asnumpy())


@pytest.mark.level0
@pytest.mark.platform_x86_cpu
@pytest.mark.env_onecard
def test_vjp_multiple_inputs_default_v_graph():
    x = Tensor(np.array([[1, 2], [3, 4]]).astype(np.float32))
    y = Tensor(np.array([[1, 2], [3, 4]]).astype(np.float32))
    v = Tensor(np.array([[1, 1], [1, 1]]).astype(np.float32))
    net = MultipleInputsOutputNet()
    expect_primal_0 = Tensor(np.array([[2, 4], [6, 8]]).astype(np.float32))
    expect_primal_1 = Tensor(np.array([[1, 8], [27, 64]]).astype(np.float32))
    expect_grad_0 = Tensor(np.array([[2, 2], [2, 2]]).astype(np.float32))
    expect_grad_1 = Tensor(np.array([[3, 12], [27, 48]]).astype(np.float32))
    primal, grad = vjp(net, (x, y), (v, v))
    assert isinstance(primal, tuple)
    assert len(primal) == 2
    assert np.allclose(primal[0].asnumpy(), expect_primal_0.asnumpy())
    assert np.allclose(primal[1].asnumpy(), expect_primal_1.asnumpy())
    assert isinstance(grad, tuple)
    assert len(grad) == 2
    assert np.allclose(grad[0].asnumpy(), expect_grad_0.asnumpy())
    assert np.allclose(grad[1].asnumpy(), expect_grad_1.asnumpy())


@pytest.mark.level0
@pytest.mark.platform_x86_cpu
@pytest.mark.env_onecard
def test_vjp_ms_function_single_input_single_output_default_v_graph():
    """
    Features: Function vjp
    Description: Test vjp with ms_function, single input, single output and default v in graph mode.
    Expectation: No exception.
    """
    x = Tensor(np.array([[1, 2], [3, 4]]).astype(np.float32))
    v = Tensor(np.array([[1, 1], [1, 1]]).astype(np.float32))
    net = SingleInputNet()

    @ms_function
    def vjp_with_ms_function(inputs, vectors):
        output, vjp_grad = vjp(net, inputs, vectors)
        return output, vjp_grad

    primal, grad = vjp_with_ms_function(x, v)
    expect_primal = Tensor(np.array([[1, 8], [27, 64]]).astype(np.float32))
    expect_grad = Tensor(np.array([[3, 12], [27, 48]]).astype(np.float32))
    assert np.allclose(primal.asnumpy(), expect_primal.asnumpy())
    assert np.allclose(grad.asnumpy(), expect_grad.asnumpy())


@pytest.mark.level0
@pytest.mark.platform_x86_cpu
@pytest.mark.env_onecard
def test_vjp_input_function_single_input_single_output_default_v_graph():
    """
    Features: Function vjp
    Description: Test vjp with function, single input, single output and default v in graph mode.
    Expectation: No exception.
    """
    x = Tensor(np.array([[1, 2], [3, 4]]).astype(np.float32))
    v = Tensor(np.array([[1, 1], [1, 1]]).astype(np.float32))

    def test_function(inputs):
        return inputs**3

    primal, grad = vjp(test_function, x, v)
    expect_primal = Tensor(np.array([[1, 8], [27, 64]]).astype(np.float32))
    expect_grad = Tensor(np.array([[3, 12], [27, 48]]).astype(np.float32))
    assert np.allclose(primal.asnumpy(), expect_primal.asnumpy())
    assert np.allclose(grad.asnumpy(), expect_grad.asnumpy())


@pytest.mark.level0
@pytest.mark.platform_x86_cpu
@pytest.mark.env_onecard
def test_vjp_construct_single_input_single_output_default_v_graph():
    """
    Features: Function vjp
    Description: Test vjp with function, single input, single output and default v in graph mode.
    Expectation: No exception.
    """
    x = Tensor(np.array([[1, 2], [3, 4]]).astype(np.float32))
    v = Tensor(np.array([[1, 1], [1, 1]]).astype(np.float32))

    class Net(nn.Module):
        def __init__(self, network):
            super(Net, self).__init__()
            self.net = network

        def call(self, inputs, vectors):
            net_out, vjp_out = vjp(self.net, inputs, vectors)
            return net_out, vjp_out

    test_net = Net(SingleInputNet())
    primal, grad = test_net(x, v)
    expect_primal = Tensor(np.array([[1, 8], [27, 64]]).astype(np.float32))
    expect_grad = Tensor(np.array([[3, 12], [27, 48]]).astype(np.float32))
    assert np.allclose(primal.asnumpy(), expect_primal.asnumpy())
    assert np.allclose(grad.asnumpy(), expect_grad.asnumpy())
