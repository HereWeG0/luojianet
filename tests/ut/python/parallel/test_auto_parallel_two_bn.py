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

import re
import numpy as np

import luojianet_ms as ms
import luojianet_ms.nn as nn
from luojianet_ms import Tensor
from luojianet_ms import context
from luojianet_ms.common.api import _cell_graph_executor
from luojianet_ms.ops import operations as P
from luojianet_ms.parallel import set_algo_parameters
from luojianet_ms.parallel._utils import _reset_op_id as reset_op_id
from tests.ut.python.ops.test_math_ops import VirtualLoss


class NetWithLoss(nn.Module):
    def __init__(self, network):
        super(NetWithLoss, self).__init__()
        self.loss = VirtualLoss()
        self.network = network

    def call(self, x):
        predict = self.network(x)
        return self.loss(predict)


class Blockcell(nn.Module):
    def __init__(self):
        super(Blockcell, self).__init__()
        self.bn = nn.BatchNorm1d(64, momentum=0.9)

    def call(self, x):
        out = self.bn(x)
        return out


def get_block():
    return Blockcell()


def test_two_bn():
    class Net(nn.Module):
        def __init__(self):
            super().__init__()
            self.block1 = get_block()
            self.block2 = get_block()
            self.relu = P.ReLU()
            self.add = P.Add()
            self.bias = Tensor(np.ones([64, 64]), dtype=ms.float32)

        def call(self, x):
            out = self.block1(x)
            out = self.relu(out)
            out = self.add(out, self.bias)
            out = self.block2(out)
            return out

    context.set_auto_parallel_context(device_num=8, global_rank=0)
    context.set_auto_parallel_context(parallel_mode="auto_parallel")
    net = NetWithLoss(Net())
    x = Tensor(np.ones([64, 64]), dtype=ms.float32)
    net.set_auto_parallel()
    net.set_train()
    set_algo_parameters(elementwise_op_strategy_follow=True)
    reset_op_id()

    _cell_graph_executor.compile(net, x, phase='train')
    strategies = _cell_graph_executor._get_shard_strategy(net)
    assert len(strategies) == 4

    for (k, v) in strategies.items():
        if re.search('BatchNorm-op', k) is not None:
            assert v == [[8, 1], [1], [1], [1], [1]]
        elif re.search('TensorAdd-op', k) is not None:
            assert v == [[8, 1], [8, 1]]
        elif re.search('ReLU-op', k) is not None:
            assert v == [[8, 1]]
