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
from luojianet_ms import context
from luojianet_ms import Tensor, nn
from luojianet_ms.common.parameter import Parameter
from luojianet_ms.ops import composite as C
from luojianet_ms.ops import operations as P
from luojianet_ms.common import dtype as mstype

grad_all = C.GradOperation(get_all=True)


@pytest.mark.level0
@pytest.mark.platform_x86_gpu_training
@pytest.mark.platform_arm_ascend_training
@pytest.mark.platform_x86_ascend_training
@pytest.mark.env_onecard
def test_for_after_for_in_for_01():
    class ForAfterForInForNet(nn.Module):
        def __init__(self):
            super().__init__()
            self.relu = nn.ReLU()
            self.softmax = nn.Softmax()
            self.mul = P.Mul()
            self.add = P.Add()
            self.sub = P.Sub()
            self.div = P.Div()
            self.assign = P.Assign()
            param_a = np.full((1,), 5, dtype=np.float32)
            self.param_a = Parameter(Tensor(param_a), name='a')
            param_b = np.full((1,), 2, dtype=np.float32)
            self.param_b = Parameter(Tensor(param_b), name='b')
            param_c = np.full((1,), 20, dtype=np.float32)
            self.param_c = Parameter(Tensor(param_c), name='c')

        def call(self, x, y):
            for _ in range(0, 4):
                self.param_b = self.add(self.param_c, self.param_b)
                for _ in range(0, 8):
                    self.param_b = self.param_a + x
                self.param_c = self.param_a * self.param_b

            for _ in range(0, 3):
                y = y + self.param_b
                x = self.relu(self.param_c * 3)
                self.param_a = x - y
            z = y + self.param_b
            return z

    class GradNet(nn.Module):
        def __init__(self, net):
            super(GradNet, self).__init__()
            self.net = net

        def call(self, *inputs):
            return grad_all(self.net)(*inputs)

    x = Tensor([11], mstype.int32)
    y = Tensor([7], mstype.int32)

    # graph mode
    context.set_context(mode=context.GRAPH_MODE)
    for_after_for_in_for_net = ForAfterForInForNet()
    net = GradNet(for_after_for_in_for_net)

    forward_net = ForAfterForInForNet()
    graph_forward_res = forward_net(x, y)
    graph_backward_res = net(x, y)

    assert graph_forward_res == Tensor([71], mstype.float32)
    assert graph_backward_res == (Tensor([0], mstype.int32), Tensor([1], mstype.int32))


@pytest.mark.level0
@pytest.mark.platform_x86_gpu_training
@pytest.mark.platform_arm_ascend_training
@pytest.mark.platform_x86_ascend_training
@pytest.mark.env_onecard
def test_for_after_for_in_for_02():
    class ForAfterForInForNet(nn.Module):
        def __init__(self):
            super().__init__()
            self.mul = P.Mul()
            self.add = P.Add()
            self.sub = P.Sub()
            self.div = P.Div()
            self.relu = nn.ReLU()
            self.assign = P.Assign()
            self.param_a = Parameter(Tensor(5, mstype.int32), name='a')
            self.param_b = Parameter(Tensor(2, mstype.int32), name='b')
            self.param_c = Parameter(Tensor(20, mstype.int32), name='c')

        def call(self, x, y):
            for _ in range(0, 6):
                self.param_b = self.add(self.param_c, self.param_b)
                for _ in range(0, 2):
                    self.assign(self.param_b, self.param_a + x)
                self.assign(self.param_c, self.param_a * self.param_b)

            for _ in range(0, 3):
                y = y + self.param_b
                x = self.relu(self.param_c * 3)
                self.assign(self.param_b, x - y)
            z = y + self.param_b
            return z

    class GradNet(nn.Module):
        def __init__(self, net):
            super(GradNet, self).__init__()
            self.net = net

        def call(self, *inputs):
            return grad_all(self.net)(*inputs)

    x = Tensor([11], mstype.int32)
    y = Tensor([7], mstype.int32)

    # graph mode
    context.set_context(mode=context.GRAPH_MODE)
    for_after_for_in_for_net = ForAfterForInForNet()
    net = GradNet(for_after_for_in_for_net)

    forward_net = ForAfterForInForNet()
    graph_forward_res = forward_net(x, y)
    graph_backward_res = net(x, y)

    assert graph_forward_res == Tensor([240], mstype.int32)
    assert graph_backward_res == (Tensor([0], mstype.int32), Tensor([1], mstype.int32))
