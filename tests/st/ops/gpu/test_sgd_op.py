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

import luojianet_ms.context as context
import luojianet_ms.nn as nn
from luojianet_ms import Tensor
from luojianet_ms.nn import Dense
from luojianet_ms.nn import TrainOneStepCell, WithLossCell
from luojianet_ms.nn.optim import SGD
from luojianet_ms.ops import operations as P

context.set_context(mode=context.GRAPH_MODE, device_target="GPU")

class NetSGD(nn.Module):
    def __init__(self):
        super(NetSGD, self).__init__()
        self.batch_size = 1
        self.reshape = P.Reshape()
        weight = Tensor(np.ones([10, 16]).astype(np.float32) * 0.01)
        self.fc1 = Dense(16, 10, weight_init=weight)

    def call(self, input_x):
        output = self.reshape(input_x, (self.batch_size, -1))
        output = self.fc1(output)
        return output


@pytest.mark.level0
@pytest.mark.platform_x86_gpu_training
@pytest.mark.env_onecard
def test_SGD():
    epoch = 3
    net = NetSGD()
    learning_rate = 0.1
    momentum = 0.9
    dampening = 0.0
    weight_decay = 0.0
    nesterov = True
    loss_scale = 1.0

    optimizer = SGD(filter(lambda x: x.requires_grad, net.get_parameters()), learning_rate, momentum, dampening,
                    weight_decay, nesterov, loss_scale)
    criterion = nn.SoftmaxCrossEntropyWithLogits(sparse=True, reduction='mean')
    net_with_criterion = WithLossCell(net, criterion)
    train_network = TrainOneStepCell(net_with_criterion, optimizer)  # optimizer
    train_network.set_train()
    losses = []
    for _ in range(epoch):
        data = Tensor(np.arange(0, 16).reshape(1, 1, 4, 4).astype(np.float32) * 0.01)
        label = Tensor(np.array([0]).astype(np.int32))
        loss = train_network(data, label)
        losses.append(loss.asnumpy())

    last_loss = 100.0
    for loss in losses:
        assert last_loss > loss
        last_loss = loss
    return losses
