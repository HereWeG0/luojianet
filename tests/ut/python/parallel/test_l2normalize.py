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
from luojianet_ms import Tensor
from luojianet_ms import context
from luojianet_ms.common.api import _cell_graph_executor
from luojianet_ms.ops import composite as C
from luojianet_ms.ops import operations as P
from tests.ut.python.ops.test_math_ops import VirtualLoss


grad_all = C.GradOperation(get_all=True)


class NetWithLoss(nn.Module):
    def __init__(self, network):
        super(NetWithLoss, self).__init__()
        self.loss = VirtualLoss()
        self.network = network

    def call(self, x, y, b):
        predict = self.network(x, y, b)
        return self.loss(predict)


class GradWrap(nn.Module):
    def __init__(self, network):
        super(GradWrap, self).__init__()
        self.network = network

    def call(self, x, y, b):
        return grad_all(self.network)(x, y, b)


# model_parallel test
def test_l2normalize_matmul():
    class Net(nn.Module):
        def __init__(self, strategy1, strategy2, strategy3):
            super().__init__()
            self.norm1 = P.L2Normalize(axis=0).shard(strategy1)
            self.norm2 = P.L2Normalize(axis=0).shard(strategy1)
            self.mul1 = P.Mul().shard(strategy2)
            self.mul2 = P.Mul().shard(strategy3)

        def call(self, x, y, b):
            y = self.norm1(y)
            x = self.norm2(x)
            out = self.mul1(x, y)
            out = self.mul2(out, b)
            return out

    context.set_auto_parallel_context(device_num=8, global_rank=0)
    strategy1 = ((1, 1, 4),)
    strategy2 = ((1, 1, 4), (1, 1, 4))
    strategy3 = ((1, 1, 8), (1, 1, 8))
    net = GradWrap(NetWithLoss(Net(strategy1, strategy2, strategy3)))
    context.set_auto_parallel_context(parallel_mode="semi_auto_parallel")
    net.set_auto_parallel()

    x = Tensor(np.ones([128, 32, 64]), dtype=ms.float32)
    y = Tensor(np.ones([128, 32, 64]), dtype=ms.float32)
    b = Tensor(np.ones([128, 32, 64]), dtype=ms.float32)
    net.set_train()
    _cell_graph_executor.compile(net, x, y, b)
