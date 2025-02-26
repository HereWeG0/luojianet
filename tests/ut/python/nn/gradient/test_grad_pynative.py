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
"""test function grad in pynative mode"""
import numpy as np
import luojianet_ms.nn as nn
import luojianet_ms.context as context
from luojianet_ms import Tensor
from luojianet_ms.ops.functional import grad

context.set_context(mode=context.PYNATIVE_MODE)


class SingleInputSingleOutputNet(nn.Module):
    def call(self, x):
        return x**3


class MultipleInputsMultipleOutputsNet(nn.Module):
    def call(self, x, y, z):
        return x**2 + y**2 + z**2, x*y*z


def function(x, y, z):
    return x**2 + y**2 + z**2, x*y*z


def test_grad_single_input_single_output_cell_pynative():
    """
    Features: Function grad.
    Description: Test F.grad with single input and single output net in pynative mode.
    Expectation: No exception.
    """
    x = Tensor(np.array([[1, 2], [3, 4]]).astype(np.float32))
    net = SingleInputSingleOutputNet()
    grad(net)(x)


def test_grad_multiple_inputs_multiple_outputs_cell_pynative():
    """
    Features: Function grad.
    Description: Test F.grad with multiple inputs and multiple outputs net in pynative mode.
    Expectation: No exception.
    """
    x = Tensor(np.array([[1, 2], [3, 4]]).astype(np.float32))
    y = Tensor(np.array([[-2, 3], [-1, 2]]).astype(np.float32))
    z = Tensor(np.array([[0, 3], [5, -1]]).astype(np.float32))
    net = MultipleInputsMultipleOutputsNet()
    grad(net, grad_position=(1, 2))(x, y, z)


def test_grad_function_with_sens_pynative():
    """
    Features: Function grad.
    Description: Test F.grad with function setting sens_param in pynative mode.
    Expectation: No exception.
    """
    x = Tensor(np.array([[1, 2], [3, 4]]).astype(np.float32))
    y = Tensor(np.array([[-2, 3], [-1, 2]]).astype(np.float32))
    z = Tensor(np.array([[0, 3], [5, -1]]).astype(np.float32))
    v = Tensor(np.array([[-1, 3], [2, 1]]).astype(np.float32))
    grad(function, grad_position=(1, 2), sens_param=True)(x, y, z, (v, v))
