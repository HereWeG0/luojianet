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

import numpy as np

import luojianet_ms as ms
import luojianet_ms.nn as nn
from luojianet_ms import Tensor, context
from luojianet_ms.common.parameter import Parameter
from luojianet_ms.nn.loss import SoftmaxCrossEntropyWithLogits
from luojianet_ms.nn.optim.momentum import Momentum
from luojianet_ms.ops import operations as P
from luojianet_ms.train import Model
from luojianet_ms.context import ParallelMode
from tests.dataset_mock import MindData


class Dataset(MindData):
    def __init__(self, predict, label, length=3):
        super(Dataset, self).__init__(size=length)
        self.predict = predict
        self.label = label
        self.index = 0
        self.length = length

    def __iter__(self):
        return self

    def __next__(self):
        if self.index >= self.length:
            raise StopIteration
        self.index += 1
        return self.predict, self.label

    def reset(self):
        self.index = 0


class TransposeNet(nn.Module):
    def __init__(self, strategy1, strategy2):
        super(TransposeNet, self).__init__()
        self.matmul = P.MatMul().shard(((8, 1), (1, 1)))
        self.matmul_weight = Parameter(Tensor(np.ones([128, 256]), dtype=ms.float32), name="weight")
        self.transpose1 = P.Transpose().shard(strategy1)
        self.transpose2 = P.Transpose().shard(strategy2)

    def call(self, x):
        x = self.matmul(x, self.matmul_weight)
        x = self.transpose1(x, (1, 0))
        x = self.transpose2(x, (1, 0))
        return x


def transpose_net(strategy1, strategy2):
    return TransposeNet(strategy1=strategy1, strategy2=strategy2)


def transpose_common(strategy1, strategy2):
    learning_rate = 0.1
    momentum = 0.9
    epoch_size = 2

    context.reset_auto_parallel_context()
    context.set_auto_parallel_context(parallel_mode=ParallelMode.SEMI_AUTO_PARALLEL, device_num=8,
                                      parameter_broadcast=False)

    predict = Tensor(np.ones([32, 128]), dtype=ms.float32)
    label = Tensor(np.ones([32]), dtype=ms.int32)
    dataset = Dataset(predict, label, 2)
    net = transpose_net(strategy1, strategy2)

    loss = SoftmaxCrossEntropyWithLogits(sparse=True, reduction='mean')
    loss.softmax_cross_entropy.shard(((8, 1), (8, 1)))
    opt = Momentum(net.trainable_params(), learning_rate, momentum)
    context.set_context(mode=context.GRAPH_MODE)
    model = Model(net, loss, opt)

    model.train(epoch_size, dataset, dataset_sink_mode=False)


def test_transpose1():
    """
    Feature: distribute operator transpose in auto parallel.
    Description: run transpose distribute operator using model.
    Expectation: compile done without error.
    """
    strategy1 = ((1, 8),)
    strategy2 = ((1, 8),)
    transpose_common(strategy1, strategy2)


def test_transpose2():
    """
    Feature: distribute operator transpose in auto parallel.
    Description: run transpose distribute operator using model.
    Expectation: compile done without error.
    """
    strategy1 = ((1, 4),)
    strategy2 = ((1, 8),)
    transpose_common(strategy1, strategy2)


if __name__ == '__main__':
    test_transpose1()
    test_transpose2()
