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
from luojianet_ms import Tensor, Parameter
from luojianet_ms import context
from luojianet_ms.ops import operations as P


class NetWithLoss(nn.Module):
    def __init__(self, network):
        super(NetWithLoss, self).__init__()
        self.loss = P.SoftmaxCrossEntropyWithLogits()
        self.network = network

    def call(self, x, b):
        predict = self.network(x)
        return self.loss(predict, b)[0]


def test_parameter_init():
    class Net(nn.Module):
        def __init__(self, strategy1, weight):
            super().__init__()
            self.weight = Parameter(weight, "w1")
            self.matmul = P.MatMul(transpose_a=False, transpose_b=True).shard(strategy1)

        def call(self, x):
            out = self.matmul(x, self.weight)
            return out

    context.set_auto_parallel_context(device_num=2, global_rank=0)
    context.set_auto_parallel_context(parallel_mode="semi_auto_parallel")
    strategy1 = ((1, 1), (2, 1))
    context.set_context(mode=context.GRAPH_MODE)

    x = Tensor(np.ones([64, 32]), dtype=ms.float32)
    weight = Tensor(np.ones([64, 32]), dtype=ms.float32)

    net = Net(strategy1, weight)
    net(x,)


if __name__ == '__main__':
    test_parameter_init()
