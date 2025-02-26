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
import pytest
import numpy as np
import luojianet_ms.ops.operations as P
from luojianet_ms.common.parameter import Parameter
from luojianet_ms import context
from luojianet_ms import Tensor
from luojianet_ms.nn import Module
from luojianet_ms.nn.optim import Momentum
from luojianet_ms.nn.wrap.cell_wrapper import WithLossCell
from luojianet_ms.nn.wrap.loss_scale import TrainOneStepWithLossScaleCell
from luojianet_ms.ops import functional as F
from luojianet_ms.common import dtype as mstype

context.set_context(mode=context.GRAPH_MODE, device_target="Ascend")


class Net(Module):
    def __init__(self, in_features, out_features):
        super(Net, self).__init__()
        self.weight = Parameter(Tensor(np.ones([out_features, in_features]).astype(np.float32)), name="weight")
        self.bias = Parameter(Tensor(np.ones([out_features]).astype(np.float32)), name="bias")
        self.matmul = P.MatMul()
        self.add = P.Add()

    def call(self, input_):
        output = self.add(self.matmul(input_, self.weight), self.bias)
        return output


def get_axis(x):
    shape_op = P.Shape()
    shape = shape_op(x)
    length = F.tuple_len(shape)
    perm = F.make_range(0, length)
    return perm


class MSELoss(Module):
    def __init__(self):
        super(MSELoss, self).__init__()
        self.reduce_sum = P.ReduceSum()
        self.square = P.Square()
        self.reduce_mean = P.ReduceMean()

    def call(self, data, label):
        diff = data - label
        return self.reduce_mean(self.square(diff), get_axis(diff))


@pytest.mark.level1
@pytest.mark.platform_arm_ascend_training
@pytest.mark.platform_x86_ascend_training
@pytest.mark.env_onecard
def test_momentum_loss():
    inputs = Tensor(np.ones([15, 1]).astype(np.float32))
    label = Tensor(np.zeros([15, 1]).astype(np.float32))
    net = Net(1, 1)
    loss = MSELoss()
    optimizer = Momentum(net.trainable_params(), learning_rate=0.1, momentum=0.9)
    net_with_loss = WithLossCell(net, loss)
    train_network = TrainOneStepWithLossScaleCell(net_with_loss, optimizer,
                                                  scale_sense=Tensor(np.full((1), 1.0), dtype=mstype.float32))
    train_network.set_train()
    output = train_network(inputs, label)
    print("the result is ", output)
