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
from luojianet_ms.common import dtype as mstype
from luojianet_ms import nn
from luojianet_ms import Tensor
from luojianet_ms.ops import composite as C
from luojianet_ms import context
from tests.security_utils import security_off_wrap

context.set_context(mode=context.GRAPH_MODE)


class ForwardNet(nn.Module):
    def __init__(self, max_cycles=10):
        super(ForwardNet, self).__init__()
        self.max_cycles = max_cycles
        self.zero = Tensor(np.array(0), mstype.int32)

    def call(self, x, y):
        out = self.zero
        for _ in range(self.max_cycles):
            out = x * y + out
            if out > 20:
                break
        return out


class BackwardNet(nn.Module):
    def __init__(self, net):
        super(BackwardNet, self).__init__(auto_prefix=False)
        self.forward_net = net
        self.grad = C.GradOperation()

    def call(self, *inputs):
        grads = self.grad(self.forward_net)(*inputs)
        return grads


@pytest.mark.level0
@pytest.mark.platform_x86_gpu_training
@pytest.mark.platform_arm_ascend_training
@pytest.mark.platform_x86_ascend_training
@pytest.mark.env_onecard
def test_forward():
    context.set_context(mode=context.GRAPH_MODE)
    x = Tensor(np.array(1), mstype.int32)
    y = Tensor(np.array(3), mstype.int32)
    forward_net = ForwardNet(max_cycles=3)
    graph_out = forward_net(x, y)

    assert graph_out == Tensor(np.array(9), mstype.int32)


@pytest.mark.level0
@pytest.mark.platform_x86_gpu_training
@pytest.mark.platform_arm_ascend_training
@pytest.mark.platform_x86_ascend_training
@pytest.mark.env_onecard
@security_off_wrap
def test_backward():
    context.set_context(mode=context.GRAPH_MODE)
    context.set_context(save_graphs=True)
    x = Tensor(np.array(1), mstype.int32)
    y = Tensor(np.array(3), mstype.int32)
    forward_net = ForwardNet(max_cycles=3)
    backward_net = BackwardNet(forward_net)
    graph_grads = backward_net(x, y)

    assert graph_grads == Tensor(np.array(9), mstype.int32)
