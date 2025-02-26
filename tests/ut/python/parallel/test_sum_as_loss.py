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


grad_all = C.GradOperation(get_all=True)


class GradWrap(nn.Module):
    def __init__(self, network):
        super(GradWrap, self).__init__()
        self.network = network

    def call(self, x, y):
        return grad_all(self.network)(x, y)


def compile_net(net, x, y):
    net.set_auto_parallel()
    net.set_train()
    _cell_graph_executor.compile(net, x, y)


def test_sum_as_loss():
    class Net(nn.Module):
        def __init__(self, strategy0, strategy1):
            super().__init__()
            self.fc_nobias = P.MatMul(transpose_b=True).shard(strategy0)
            self.reduce_sum = P.ReduceSum(keep_dims=False).shard(strategy1)

        def call(self, x, y):
            out = self.fc_nobias(x, y)
            out = self.reduce_sum(out, (0, 1))
            return out

    context.set_auto_parallel_context(device_num=16, global_rank=0)
    strategy0 = ((4, 1), (4, 1))
    strategy1 = ((4, 1),)
    net = GradWrap(Net(strategy0, strategy1))
    context.set_auto_parallel_context(parallel_mode="semi_auto_parallel")

    x = Tensor(np.ones([64, 32]), dtype=ms.float32)
    y = Tensor(np.ones([64, 32]), dtype=ms.float32)
    compile_net(net, x, y)


def test_sum_as_loss2():
    class Net(nn.Module):
        def __init__(self, strategy0, strategy1):
            super().__init__()
            self.fc_nobias = P.MatMul(transpose_b=True).shard(strategy0)
            self.reduce_sum = P.ReduceSum(keep_dims=False).shard(strategy1)

        def call(self, x, y):
            out = self.fc_nobias(x, y)
            out = self.reduce_sum(out, (0, 1))
            return out

    context.set_auto_parallel_context(device_num=16, global_rank=0)
    strategy0 = ((4, 1), (4, 1))
    strategy1 = ((1, 1),)
    net = GradWrap(Net(strategy0, strategy1))
    context.set_auto_parallel_context(parallel_mode="semi_auto_parallel")

    x = Tensor(np.ones([64, 32]), dtype=ms.float32)
    y = Tensor(np.ones([64, 32]), dtype=ms.float32)
    compile_net(net, x, y)
