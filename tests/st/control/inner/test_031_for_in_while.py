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
from tests.security_utils import security_off_wrap

grad_all = C.GradOperation(get_all=True)


@pytest.mark.skip(reason="not supported for in while")
def test_for_in_while_01():
    class ForInWhileNet(nn.Module):
        def __init__(self):
            super().__init__()
            self.mul = P.Mul()
            self.add = P.Add()
            self.sub = P.Sub()
            self.assign = P.Assign()
            param_a = np.full((1,), 5, dtype=np.float32)
            self.param_a = Parameter(Tensor(param_a), name='a')
            param_b = np.full((1,), 2, dtype=np.float32)
            self.param_b = Parameter(Tensor(param_b), name='b')

        def call(self, x):
            self.assign(self.param_a, x + self.param_a)
            while self.param_a > self.param_b:
                x = self.mul(x, 2)
                for _ in range(0, 5):
                    x = self.add(x, x)
                    self.param_b = self.param_b + 1
            y = self.sub(x, self.param_b)
            self.assign(self.param_a, y)
            return x

    class GradNet(nn.Module):
        def __init__(self, net):
            super(GradNet, self).__init__()
            self.net = net

        def call(self, *inputs):
            return grad_all(self.net)(*inputs)

    x = Tensor([2], mstype.int32)

    # graph mode
    context.set_context(mode=context.GRAPH_MODE)
    for_in_while_net = ForInWhileNet()
    backward_net = GradNet(for_in_while_net)

    forward_net = ForInWhileNet()
    graph_forward_res = forward_net(x)
    graph_backward_res = backward_net(x)

    expect_forward_res = 0
    expect_backward_res = 0
    assert graph_forward_res == expect_forward_res
    assert graph_backward_res == expect_backward_res


@pytest.mark.skip(reason="not supported for in while")
@security_off_wrap
def test_for_in_while_02():
    class ForInWhileNet(nn.Module):
        def __init__(self):
            super().__init__()
            self.mul = P.Mul()
            self.add = P.Add()
            self.sub = P.Sub()
            self.assign = P.Assign()
            self.param_a = Parameter(Tensor(5, mstype.int32), name='a')
            self.param_b = Parameter(Tensor(7, mstype.int32), name='b')

        def call(self, x):
            self.assign(self.param_a, x + self.param_a)
            while self.param_a > self.param_b:
                for _ in range(0, 3):
                    x = self.add(x, self.param_a + self.param_b)
                    self.assign(self.param_b, self.param_b + 1)
            y = self.sub(x, self.param_b)
            self.assign(self.param_a, y)
            return x

    class GradNet(nn.Module):
        def __init__(self, net):
            super(GradNet, self).__init__()
            self.net = net

        def call(self, *inputs):
            return grad_all(self.net)(*inputs)

    x = Tensor([2], mstype.int32)

    # graph mode
    context.set_context(mode=context.GRAPH_MODE)
    context.set_context(save_graphs=True)
    for_in_while_net = ForInWhileNet()
    net = GradNet(for_in_while_net)
    graph_forward_res = for_in_while_net(x)
    graph_backward_res = net(x)

    expect_forward_res = 0
    expect_backward_res = 0
    assert graph_forward_res == expect_forward_res
    assert graph_backward_res == expect_backward_res
