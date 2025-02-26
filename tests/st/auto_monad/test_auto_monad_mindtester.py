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
# ==============================================================================
import os
import pytest
import numpy as np
import luojianet_ms as ms
import luojianet_ms.ops.operations as P
from luojianet_ms.nn import Module
from luojianet_ms import context, Tensor
from luojianet_ms.common.parameter import Parameter
from luojianet_ms.common.initializer import initializer
from luojianet_ms.train.model import Model
from luojianet_ms.ops.composite import GradOperation
from luojianet_ms.common import ParameterTuple
from tests.security_utils import security_off_wrap

context.set_context(mode=context.GRAPH_MODE)


class _Grad(Module):
    def __init__(self, grad, network, wrt_params=False, real_inputs_count=None):
        super().__init__()
        self.network = network
        self.grad = grad
        self.sens_param = self.grad.sens_param
        self.wrt_params = wrt_params
        self.real_inputs_count = real_inputs_count
        if self.wrt_params:
            self.params = ParameterTuple(self.network.trainable_params())

    def call(self, *inputs):
        if self.real_inputs_count is None or self.sens_param is False:
            if self.wrt_params:
                return self.grad(self.network, self.params)(*inputs)
            return self.grad(self.network)(*inputs)

        real_inputs = inputs[:self.real_inputs_count]
        sense_param_inputs = inputs[self.real_inputs_count:]
        if self.wrt_params:
            return self.grad(self.network, self.params)(*real_inputs, sense_param_inputs)
        return self.grad(self.network)(*real_inputs, sense_param_inputs)


class GradOfFirstInput(_Grad):
    """
    get grad of first input
    """

    def __init__(self, network, sens_param=True, real_inputs_count=None):
        super().__init__(grad=GradOperation(sens_param=sens_param),
                         network=network, real_inputs_count=real_inputs_count)


class GradOfAllInputs(_Grad):
    '''
    get grads of all inputs
    '''

    def __init__(self, network, sens_param=True, real_inputs_count=None):
        super().__init__(grad=GradOperation(get_all=True, sens_param=sens_param),
                         network=network, real_inputs_count=real_inputs_count)


class GradOfAllInputsAndParams(_Grad):
    '''
    get grads of all inputs and params
    '''

    def __init__(self, network, sens_param=True, real_inputs_count=None):
        super().__init__(grad=GradOperation(get_all=True, get_by_list=True, sens_param=sens_param),
                         network=network, wrt_params=True, real_inputs_count=real_inputs_count)


def _count_unequal_element(data_expected, data_me, rtol, atol):
    assert data_expected.shape == data_me.shape
    total_count = len(data_expected.flatten())
    error = np.abs(data_expected - data_me)
    greater = np.greater(error, atol + np.abs(data_me) * rtol)
    loss_count = np.count_nonzero(greater)
    assert (loss_count / total_count) < rtol, \
        "\ndata_expected_std:{0}\ndata_me_error:{1}\nloss:{2}". \
            format(data_expected[greater], data_me[greater], error[greater])


def allclose_nparray(data_expected, data_me, rtol, atol, equal_nan=True):
    if np.any(np.isnan(data_expected)):
        assert np.allclose(data_expected, data_me, rtol,
                           atol, equal_nan=equal_nan)
    elif not np.allclose(data_expected, data_me, rtol, atol, equal_nan=equal_nan):
        _count_unequal_element(data_expected, data_me, rtol, atol)
    else:
        assert True


class ControlGraphSupportNotEqual(Module):
    def call(self, x, y, z, input_data):
        if x != y:
            out = input_data + input_data
        else:
            out = input_data - input_data
        if x == z:
            out2 = input_data * input_data
        else:
            out2 = input_data / input_data
        if x == z:
            out3_f = (lambda a: a + a)
            out3 = out3_f(input_data)
        else:
            out3_f = (lambda a: a + a + a)
            out3 = out3_f(input_data)
        return out, out2, out3


@pytest.mark.level1
@pytest.mark.platform_arm_ascend_training
@pytest.mark.platform_x86_ascend_training
@pytest.mark.env_onecard
def test_ctrl_if_while_graph_support_not_equal_true():
    x = np.array(0).astype(np.float32)
    y = np.array(3).astype(np.float32)
    input_shape = (512, 512, 7, 7)
    input_data = np.random.randn(*input_shape).astype(np.float32)
    net = ControlGraphSupportNotEqual()
    model = Model(net)
    out_me = model.predict(Tensor(x), Tensor(y), Tensor(x), Tensor(input_data))
    out = input_data + input_data
    out2 = input_data * input_data
    out3 = input_data + input_data
    allclose_nparray(out, out_me[0].asnumpy(), 0.0001, 0.0001)
    allclose_nparray(out2, out_me[1].asnumpy(), 0.0001, 0.0001)
    allclose_nparray(out3, out_me[2].asnumpy(), 0.0001, 0.0001)


@pytest.mark.level1
@pytest.mark.platform_arm_ascend_training
@pytest.mark.platform_x86_ascend_training
@pytest.mark.env_onecard
def test_ctrl_if_while_graph_support_not_equal_false():
    x = np.array(0).astype(np.float32)
    y = np.array(0).astype(np.float32)
    z = np.array(3).astype(np.float32)
    input_shape = (512, 512, 7, 7)
    input_data = np.random.randn(*input_shape).astype(np.float32)
    net = ControlGraphSupportNotEqual()
    model = Model(net)
    out_me = model.predict(Tensor(x), Tensor(y), Tensor(z), Tensor(input_data))
    out = input_data - input_data
    out2 = input_data / input_data
    out3 = input_data + input_data + input_data
    allclose_nparray(out, out_me[0].asnumpy(), 0.0001, 0.0001)
    allclose_nparray(out2, out_me[1].asnumpy(), 0.0001, 0.0001)
    allclose_nparray(out3, out_me[2].asnumpy(), 0.0001, 0.0001)


class ControlBprop(Module):
    def call(self, x, y, z, input_data):
        if x != y:
            out = input_data + input_data
        else:
            out = input_data - input_data
        if x == z:
            out2 = input_data * input_data
        else:
            out2 = input_data / input_data
        if x == z:
            out3_f = (lambda a: a + a)
            out3 = out3_f(input_data)
        else:
            out3_f = (lambda a: a + a + a)
            out3 = out3_f(input_data)
        return out, out2, out3

    def bprop(self, x, y, z, input_data, out, dout):
        return x * 2, y * 3, z, input_data * 5.1


@pytest.mark.level1
@pytest.mark.platform_arm_ascend_training
@pytest.mark.platform_x86_ascend_training
@pytest.mark.env_onecard
def test_ctrl_if_while_bprop_true():
    x = np.array(0).astype(np.float32)
    y = np.array(3).astype(np.float32)
    input_shape = (512, 512, 7, 7)
    input_data = np.random.randn(*input_shape).astype(np.float32)
    net = ControlBprop()
    grad_net = GradOfAllInputs(net, sens_param=False)
    grad_net.set_train()
    grads = grad_net(Tensor(x), Tensor(y), Tensor(x), Tensor(input_data))
    allclose_nparray(x * 2, grads[0].asnumpy(), 0.0000, 0.0000)
    allclose_nparray(y * 3, grads[1].asnumpy(), 0.0000, 0.0000)
    allclose_nparray(x, grads[2].asnumpy(), 0.0000, 0.0000)
    allclose_nparray(input_data * 5.1, grads[3].asnumpy(), 0.0000, 0.0000)


class TwoInput(Module):
    def __init__(self):
        super().__init__()
        self.op = P.Mul()

    def call(self, x, y):
        x = self.op(x, y)
        return x


class InlineBpropTwoInput1(Module):
    def __init__(self):
        super().__init__()
        self.f = TwoInput()
        self.f.set_grad()
        self.grad = GradOfAllInputs(self.f, sens_param=False)

    def call(self, x, y):
        if x > y:
            x = self.f(x, y)
        else:
            x = self.f(x, y)
        return x

    def bprop(self, x, y, out, dout):
        if x > y:
            grads = self.grad(x, y)
        else:
            grads = self.grad(x, y)
        return grads[0] * 2, grads[1] * 2


@pytest.mark.level1
@pytest.mark.platform_arm_ascend_training
@pytest.mark.platform_x86_ascend_training
@pytest.mark.env_onecard
def test_ctrl_if_while_bprop_inlinebprop_twoinput():
    net = InlineBpropTwoInput1()
    input1 = Tensor(np.array(2).astype(np.float32))
    input2 = Tensor(np.array(1).astype(np.float32))
    grad_net = GradOfAllInputs(net, sens_param=False)
    grad_net.set_train()
    grads = grad_net(input1, input2)
    allclose_nparray(input1.asnumpy() * 2, grads[1].asnumpy(), 0, 0)
    allclose_nparray(input2.asnumpy() * 2, grads[0].asnumpy(), 0, 0)


class ControlOneIfOneParaOneAddn(Module):
    def __init__(self, input_shape):
        super().__init__()
        self.addn = P.AddN()
        self.assign = P.Assign()
        self.inputdata = Parameter(initializer(
            1, input_shape, ms.float32), name="global_step")

    def call(self, x, y, input_data):
        if x > y:
            out = self.inputdata
        else:
            out = self.addn([input_data, input_data, input_data])
        if x > y:
            out = self.assign(self.inputdata, input_data)
        return out


@pytest.mark.level0
@pytest.mark.platform_arm_ascend_training
@pytest.mark.platform_x86_ascend_training
@pytest.mark.env_onecard
def test_ctrl_if_para_addn_true():
    x = Tensor(1, ms.float32)
    y = Tensor(0, ms.float32)
    input_shape = (1024, 512, 7, 7)
    input_data = np.random.randn(*input_shape).astype(np.float32)
    net = ControlOneIfOneParaOneAddn(input_shape)
    out = net(x, y, Tensor(input_data))
    allclose_nparray(input_data[0], out.asnumpy()[0], 0.0001, 0.0001)


class AddnCell(Module):
    def __init__(self):
        super().__init__()
        self.addn = P.AddN()

    def call(self, x):
        x = self.addn((x, x))
        return x


class SideEffectMemoryCellAddnNet(Module):
    def __init__(self):
        super().__init__()
        self.para = Parameter(Tensor([1.0], ms.float32), name="para")
        self.assign = P.Assign()
        self.addn = P.AddN()
        self.addn1 = AddnCell()

    def call(self, x):
        x = self.addn1(x)
        self.assign(self.para, x)
        out = self.addn((self.para, x))
        return out

    def grad_luojianet_ms_impl(self, params, grad_ys):
        grad_net = GradOfAllInputsAndParams(self)
        grad_net.set_train()
        grad_out = grad_net(params, grad_ys)
        return grad_out


@pytest.mark.level1
@pytest.mark.platform_arm_ascend_training
@pytest.mark.platform_x86_ascend_training
@pytest.mark.env_onecard
def test_grad_memory_addn():
    net = SideEffectMemoryCellAddnNet()
    grad_ys = Tensor([18.0], ms.float32)
    inputs = Tensor([9.0], ms.float32)
    net.grad_luojianet_ms_impl(inputs, grad_ys)


class SideEffectIOCellAddnNet(Module):
    def __init__(self):
        super().__init__()
        self.para1 = Parameter(Tensor([1.0], ms.float32), name="para1")
        self.para2 = Parameter(Tensor([3.0], ms.float32), name="para2")
        self.print = P.Print()
        self.addn = AddnCell()

    def call(self, x):
        self.print("para1:", self.para1)
        self.print("para2:", self.para2)
        x = self.addn(x)
        return x

    def grad_luojianet_ms_impl(self, params, grad_ys):
        grad_net = GradOfAllInputsAndParams(self)
        grad_net.set_train()
        grad_out = grad_net(params, grad_ys)
        return grad_out


@security_off_wrap
@pytest.mark.level1
@pytest.mark.platform_arm_ascend_training
@pytest.mark.platform_x86_ascend_training
@pytest.mark.env_onecard
def test_grad_io_addn():
    net = SideEffectIOCellAddnNet()
    grad_ys = Tensor([18.0], ms.float32)
    inputs = Tensor([9.0], ms.float32)
    net.grad_luojianet_ms_impl(inputs, grad_ys)


class SideEffectReturnParameterNet(Module):
    def __init__(self):
        super().__init__()
        self.para = Parameter(Tensor([1.0], ms.float32), name="para")
        self.assign = P.Assign()
        self.addn = P.AddN()
        self.relu = P.ReLU()

    def call(self, inputs):
        p1 = self.assign(self.para, inputs)
        out = self.addn((inputs, inputs, inputs))
        out = self.relu(out)
        return p1

    def grad_luojianet_ms_impl(self, params, grad_ys):
        grad_net = GradOfAllInputsAndParams(self)
        grad_net.set_train()
        grad_out = grad_net(params, grad_ys)
        return grad_out


@pytest.mark.level1
@pytest.mark.platform_arm_ascend_training
@pytest.mark.platform_x86_ascend_training
@pytest.mark.env_onecard
def test_grad_read_dependency_return_parameter():
    net = SideEffectReturnParameterNet()
    grad_ys = Tensor([18.0], ms.float32)
    inputs = Tensor([9.0], ms.float32)
    net.grad_luojianet_ms_impl(inputs, grad_ys)


class SideEffectAssignAddnReluReturnParNet(Module):
    def __init__(self):
        super().__init__()
        self.parameter1 = Parameter(
            Tensor([1.0], ms.float32), name="parameter1")
        self.assign = P.Assign()
        self.addN = P.AddN()
        self.relu = P.ReLU()

    def call(self, inputs):
        p1 = self.assign(self.parameter1, inputs)
        out = self.addN((inputs, inputs, inputs))
        out = self.relu(out)
        return p1

    def grad_luojianet_ms_impl(self, params, grad_ys):
        grad_net = GradOfAllInputsAndParams(self)
        grad_net.set_train()
        grad_out = grad_net(params, grad_ys)
        return grad_out


@pytest.mark.level1
@pytest.mark.platform_arm_ascend_training
@pytest.mark.platform_x86_ascend_training
@pytest.mark.env_onecard
def test_side_effect_grad_read_dependency_assign_addn_relu_return_parameter():
    net = SideEffectAssignAddnReluReturnParNet()
    grad_ys = Tensor([18.0], ms.float32)
    inputs = Tensor([9.0], ms.float32)
    out1 = net.grad_luojianet_ms_impl(inputs, grad_ys)
    net = SideEffectAssignAddnReluReturnParNet()
    try:
        context.set_context(mode=context.PYNATIVE_MODE)
        out2 = net.grad_luojianet_ms_impl(inputs, grad_ys)
        allclose_nparray(out1[0][0].asnumpy(), out2[0]
                         [0].asnumpy(), 0.001, 0.001)
        allclose_nparray(out1[1][0].asnumpy(), out2[1]
                         [0].asnumpy(), 0.001, 0.001)
    finally:
        context.set_context(mode=context.GRAPH_MODE)


class SideEffectPrintInHighOrdeAddnNet(Module):
    def __init__(self):
        super().__init__()
        self.parameter1 = Parameter(
            Tensor([1.0], ms.float32), name="parameter1")
        self.parameter2 = Parameter(
            Tensor([3.0], ms.float32), name="parameter2")
        self.assign = P.Assign()
        self.addn = P.AddN()
        self.mul = P.Mul()
        self.print = P.Print()

    def call(self, x):
        self.high_order_func()
        out = self.addn((self.parameter1, x, self.parameter2))
        return out

    def high_order_func(self):
        self.print("parameter1: ", self.parameter1)
        self.print("parameter2: ", self.parameter2)
        return True

    def grad_luojianet_ms_impl(self, params, grad_ys):
        grad_net = GradOfAllInputsAndParams(self)
        grad_net.set_train()
        grad_out = grad_net(params, grad_ys)
        return grad_out

@security_off_wrap
@pytest.mark.level1
@pytest.mark.platform_arm_ascend_training
@pytest.mark.platform_x86_ascend_training
@pytest.mark.env_onecard
def test_side_effect_high_order_print_in_high_order_net():
    print_file = os.getcwd() + "/test_side_effect_high_order_print_in_high_order_net.data"
    context.set_context(print_file_path=print_file)
    net = SideEffectPrintInHighOrdeAddnNet()
    out1 = net(Tensor([9.0], ms.float32))
    net = SideEffectPrintInHighOrdeAddnNet()
    try:
        context.set_context(mode=context.PYNATIVE_MODE)
        out2 = net(Tensor([9.0], ms.float32))
        allclose_nparray(out1.asnumpy(), out2.asnumpy(), 0.001, 0.001)
    finally:
        context.set_context(mode=context.GRAPH_MODE)


class SideEffectControlFlowAssignDependTwoIfNet(Module):
    def __init__(self):
        super().__init__()
        self.parameter1 = Parameter(
            Tensor([3.0], ms.float32), name="parameter1")
        self.assign = P.Assign()
        self.mul = P.Mul()
        self.addn = P.AddN()
        self.depend = P.Depend()

    def call(self, x, y):
        self.assign(self.parameter1, x)
        if self.parameter1 > y:
            x = self.mul(x, x)
            p2 = self.assign(self.parameter1, x)
            if self.parameter1 > y:
                x = self.addn((x, self.parameter1))
                p3 = self.assign(self.parameter1, x)
                self.depend(p3, p2)
        return x

    def grad_luojianet_ms_impl(self, params1, params2, grad_ys):
        grad_net = GradOfAllInputsAndParams(self)
        grad_net.set_train()
        grad_out = grad_net(params1, params2, grad_ys)
        return grad_out


@pytest.mark.level1
@pytest.mark.platform_arm_ascend_training
@pytest.mark.platform_x86_ascend_training
@pytest.mark.env_onecard
def test_side_effect_grad_control_flow_assign_depend_of_two_if():
    net = SideEffectControlFlowAssignDependTwoIfNet()
    grad_ys = Tensor([18.0], ms.float32)
    inputs1 = Tensor([9.0], ms.float32)
    inputs2 = Tensor([6.0], ms.float32)
    net.grad_luojianet_ms_impl(inputs1, inputs2, grad_ys)


class SideEffectTwoAddnSwitchNet(Module):
    def __init__(self):
        super().__init__()
        self.addN = P.AddN()

    def call(self, x):
        y = x
        x = self.addN((x, x, x))
        y = self.addN((y, y))
        if x > y:
            return x
        return y

    def grad_luojianet_ms_impl(self, params, grad_ys):
        grad_net = GradOfAllInputsAndParams(self)
        grad_net.set_train()
        grad_out = grad_net(params, grad_ys)
        return grad_out


@pytest.mark.level1
@pytest.mark.platform_arm_ascend_training
@pytest.mark.platform_x86_ascend_training
@pytest.mark.env_onecard
def test_side_effect_grad_two_addn_switch():
    net = SideEffectTwoAddnSwitchNet()
    grad_ys = Tensor([18.0], ms.float32)
    inputs = Tensor([9.0], ms.float32)
    out1 = net.grad_luojianet_ms_impl(inputs, grad_ys)
    net = SideEffectTwoAddnSwitchNet()
    try:
        expect = 54.0
        allclose_nparray(out1[0][0].asnumpy(), expect, 0.001, 0.001)
    finally:
        context.set_context(mode=context.GRAPH_MODE)


class SideEffectGradIfNet(Module):
    def __init__(self):
        super().__init__()
        self.relu = P.ReLU()
        a = np.full((1,), 5, dtype=np.float32)
        self.a = Parameter(Tensor(a), name="a")
        b = np.full((1,), 4, dtype=np.float32)
        self.b = Parameter(Tensor(b), name="b")

    def call(self, x):
        if self.a > self.b:
            x = self.relu(x)
            out = x
        else:
            out = x + 2
        return out

    def grad_luojianet_ms_impl(self, params, grad_ys):
        grad_net = GradOfFirstInput(self)
        grad_net.set_train()
        grad_out = grad_net(params, grad_ys)
        return grad_out


@pytest.mark.level1
@pytest.mark.platform_arm_ascend_training
@pytest.mark.platform_x86_ascend_training
@pytest.mark.env_onecard
def test_side_effect_grad_if():
    context.set_context(mode=context.GRAPH_MODE)
    net = SideEffectGradIfNet()
    grad_ys = Tensor([18.0], ms.float32)
    inputs = Tensor([9.0], ms.float32)
    out1 = net.grad_luojianet_ms_impl(inputs, grad_ys)
    net = SideEffectGradIfNet()
    try:
        expect = 18.0
        allclose_nparray(out1.asnumpy(), expect, 0.001, 0.001)
    finally:
        context.set_context(mode=context.GRAPH_MODE)


class OneInputBprop(Module):
    def __init__(self):
        super().__init__()
        self.op = P.ReLU()

    def call(self, x):
        return self.op(x)

    def bprop(self, x, out, dout):
        return (5 * x,)


class HighGrad(Module):
    def __init__(self, network, grad_list, sens_param=False, real_inputs_count=None):
        super().__init__()
        self.grads = [network]
        for i in range(len(grad_list) - 1):
            _grad = grad_list[i](self.grads[i], sens_param=False)
            self.grads.append(_grad)
        self.final_grad = grad_list[-1](self.grads[-1],
                                        sens_param=sens_param, real_inputs_count=real_inputs_count)

    def call(self, *inputs):
        return self.final_grad(*inputs)


@pytest.mark.level0
@pytest.mark.platform_arm_ascend_training
@pytest.mark.platform_x86_ascend_training
@pytest.mark.env_onecard
def test_highgrad_one_input_sec_grad():
    net = OneInputBprop()
    x = Tensor(np.array([2, 2]).astype(np.float32))
    grad_net = HighGrad(net, [GradOfFirstInput, GradOfFirstInput])
    dxdx = grad_net(x)
    assert (dxdx.asnumpy() == np.array([5, 5]).astype(np.float32)).all()


@pytest.mark.level1
@pytest.mark.platform_arm_ascend_training
@pytest.mark.platform_x86_ascend_training
@pytest.mark.env_onecard
def test_highgrad_one_input_third_grad():
    net = OneInputBprop()
    x = Tensor(np.array([2, 2]).astype(np.float32))
    grad_net = HighGrad(
        net, [GradOfFirstInput, GradOfFirstInput, GradOfFirstInput])
    third_grad = grad_net(x)
    assert (third_grad.asnumpy() == np.array([0, 0]).astype(np.float32)).all()


class SideEffectControlFlowAssignDependWhileNet(Module):
    def __init__(self):
        super().__init__()
        self.parameter1 = Parameter(Tensor([199.0], ms.float32), name="parameter1")
        self.assign = P.Assign()
        self.assignadd = P.AssignAdd()
        self.addn = P.AddN()
        self.depend = P.Depend()

    def call(self, x, y, z):
        p1 = self.assign(self.parameter1, x)
        while self.parameter1 < y:
            x = self.addn((x, x))
            p2 = self.assignadd(self.parameter1, z)
            self.depend(p2, p1)
        return x

    def grad_luojianet_ms_impl(self, params1, params2, params3, grad_ys):
        grad_net = GradOfAllInputsAndParams(self)
        grad_net.set_train()
        grad_out = grad_net(params1, params2, params3, grad_ys)
        return grad_out


@pytest.mark.level0
@pytest.mark.platform_arm_ascend_training
@pytest.mark.platform_x86_gpu_training
@pytest.mark.env_onecard
def test_side_effect_grad_control_flow_assign_depend_while_net():
    context.set_context(mode=context.GRAPH_MODE)
    net = SideEffectControlFlowAssignDependWhileNet()
    grad_ys = Tensor([18.0], ms.float32)
    inputs1 = Tensor([9.0], ms.float32)
    inputs2 = Tensor([6.0], ms.float32)
    inputs3 = Tensor([3.0], ms.float32)
    out1 = net.grad_luojianet_ms_impl(inputs1, inputs2, inputs3, grad_ys)

    try:
        expect1 = 18.0
        expect2 = 0
        allclose_nparray(out1[0][0].asnumpy(), expect1, 0.001, 0.001)
        allclose_nparray(out1[1][0].asnumpy(), expect2, 0.001, 0.001)
    finally:
        context.set_context(mode=context.GRAPH_MODE)


class AssignInZipLoop(Module):
    def __init__(self):
        super().__init__()
        self.conv1 = ms.nn.Conv2d(3, 2, 1, weight_init="zero")
        self.conv2 = ms.nn.Conv2d(3, 2, 1, weight_init="zero")
        self.params1 = self.conv1.trainable_params()
        self.params2 = self.conv2.trainable_params()

    def call(self, x):
        for p1, p2 in zip(self.params1, self.params2):
            P.Assign()(p2, p1 + x)

        out = 0
        for p1, p2 in zip(self.params1, self.params2):
            out = p1 + p2
            print(p1)
            print(p2)

        return out


@pytest.mark.level1
@pytest.mark.platform_arm_ascend_training
@pytest.mark.platform_x86_gpu_training
@pytest.mark.env_onecard
def test_assign_in_zip_loop():
    """
    Feature: Auto-monad load grouping and merge.
    Description: Assign/Load inside a zip loop.
    Expectation: 'p1 + p2' should be executed after Assign, and out is 1.
    """
    x = Tensor.from_numpy(np.ones([1], np.float32))
    net = AssignInZipLoop()
    out = net(x)
    assert np.all(out.asnumpy() == 1)


@pytest.mark.level1
@pytest.mark.platform_arm_ascend_training
@pytest.mark.platform_x86_gpu_training
@pytest.mark.env_onecard
def test_parameter():
    """
    Feature: Check the names of parameters.
    Description: If parameter in list or tuple is not given a name, will give it a unique name.
    Expectation: No exception.
    """
    class ParamNet(Module):
        def __init__(self):
            super(ParamNet, self).__init__()
            self.param_a = Parameter(Tensor([1], ms.float32), name="name_a")
            self.param_b = Parameter(Tensor([2], ms.float32), name="name_b")
            self.param_c = Parameter(Tensor([3], ms.float32))
            self.param_d = Parameter(Tensor([4], ms.float32))
            self.param_tuple = (Parameter(Tensor([5], ms.float32)),
                                Parameter(Tensor([6], ms.float32)))
            self.param_list = [Parameter(Tensor([5], ms.float32)),
                               Parameter(Tensor([6], ms.float32))]

        def call(self, x):
            res1 = self.param_a + self.param_b + self.param_c + self.param_d
            res1 = res1 - self.param_list[0] + self.param_list[1] + x
            res2 = self.param_list[0] + self.param_list[1]
            return res1, res2

    net = ParamNet()
    x = Tensor([10], ms.float32)
    output1, output2 = net(x)
    output1_expect = Tensor(21, ms.float32)
    output2_expect = Tensor(11, ms.float32)
    assert output1 == output1_expect
    assert output2 == output2_expect


@pytest.mark.level1
@pytest.mark.platform_arm_ascend_training
@pytest.mark.platform_x86_gpu_training
@pytest.mark.env_onecard
def test_parameter_same_name_between_tuple_or_list():
    """
    Feature: Check the names of parameters between tuple or list.
    Description: If the same name exists between tuple and list, an exception will be thrown.
    Expectation: Get the expected exception report.
    """
    class ParamNet(Module):
        def __init__(self):
            super(ParamNet, self).__init__()
            self.param_tuple = (Parameter(Tensor([1], ms.float32), name="name_a"),
                                Parameter(Tensor([2], ms.float32)))
            self.param_list = [Parameter(Tensor([3], ms.float32), name="name_a"),
                               Parameter(Tensor([4], ms.float32))]

        def call(self, x):
            res = self.param_tuple[0] + self.param_tuple[1] + self.param_list[0] + self.param_listp[1] + x
            return res

    with pytest.raises(ValueError, match="its name 'name_a' already exists."):
        net = ParamNet()
        x = Tensor([10], ms.float32)
        output = net(x)
        output_expect = Tensor(20, ms.float32)
        assert output == output_expect
