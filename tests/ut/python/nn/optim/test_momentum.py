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
""" test momentum """
import numpy as np

import luojianet_ms.nn as nn
from luojianet_ms import Tensor, Parameter
from luojianet_ms.common.api import _cell_graph_executor
from luojianet_ms.nn import TrainOneStepCell, WithLossCell
from luojianet_ms.nn.optim import Momentum
from luojianet_ms.ops import operations as P


class Net(nn.Module):
    """ Net definition """

    def __init__(self):
        super(Net, self).__init__()
        self.weight = Parameter(Tensor(np.ones([64, 10]).astype(np.float32)), name="weight")
        self.bias = Parameter(Tensor(np.ones([10]).astype(np.float32)), name="bias")
        self.matmul = P.MatMul()
        self.biasAdd = P.BiasAdd()

    def call(self, x):
        x = self.biasAdd(self.matmul(x, self.weight), self.bias)
        return x


def test_momentum_compile():
    """ test_momentum_compile """
    inputs = Tensor(np.ones([1, 64]).astype(np.float32))
    label = Tensor(np.zeros([1, 10]).astype(np.float32))
    net = Net()
    net.set_train()

    loss = nn.SoftmaxCrossEntropyWithLogits(sparse=False)
    optimizer = Momentum(net.trainable_params(), learning_rate=0.1, momentum=0.9)

    net_with_loss = WithLossCell(net, loss)
    train_network = TrainOneStepCell(net_with_loss, optimizer)
    _cell_graph_executor.compile(train_network, inputs, label)
