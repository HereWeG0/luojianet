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

import luojianet_ms.context as context
import luojianet_ms.nn as nn
from luojianet_ms import Tensor, Parameter
from luojianet_ms.communication.management import init
from luojianet_ms.ops import operations as P
from luojianet_ms.communication._comm_helper import GlobalComm

class DataParallelNet(nn.Module):
    def __init__(self):
        super(DataParallelNet, self).__init__()
        weight_init = np.random.rand(512, 64).astype(np.float32)
        self.weight = Parameter(Tensor(weight_init), name="weight", layerwise_parallel=False)
        self.fc = P.MatMul()

    def call(self, x):
        x = self.fc(x, self.weight)
        return x


class ModelParallelNet(nn.Module):
    def __init__(self):
        super(ModelParallelNet, self).__init__()
        weight_init = np.random.rand(512, 64).astype(np.float32)
        self.weight = Parameter(Tensor(weight_init), name="weight", layerwise_parallel=True)
        self.fc = P.MatMul()

    def call(self, x):
        x = self.fc(x, self.weight)
        return x


def test_param_broadcast():
    context.set_context(mode=context.GRAPH_MODE)
    context.reset_auto_parallel_context()
    context.set_auto_parallel_context(parallel_mode="data_parallel", parameter_broadcast=True)
    GlobalComm.CHECK_ENVS = False
    init()
    GlobalComm.CHECK_ENVS = True
    network = DataParallelNet()
    network.set_train()

    predict = Tensor(np.ones([64, 512]).astype(np.float32) * 0.01)
    _ = network(predict)
    context.reset_auto_parallel_context()


def test_param_not_broadcast():
    context.set_context(mode=context.GRAPH_MODE)
    context.reset_auto_parallel_context()
    context.set_auto_parallel_context(parallel_mode="data_parallel", parameter_broadcast=False)
    GlobalComm.CHECK_ENVS = False
    init()
    GlobalComm.CHECK_ENVS = True
    network = ModelParallelNet()
    network.set_train()

    predict = Tensor(np.ones([64, 512]).astype(np.float32) * 0.01)
    _ = network(predict)
    context.reset_auto_parallel_context()
