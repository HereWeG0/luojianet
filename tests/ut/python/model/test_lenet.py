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
"""test lenet"""
import numpy as np

import luojianet_ms.context as context
import luojianet_ms.nn as nn
from luojianet_ms import Tensor
from luojianet_ms.common.api import _cell_graph_executor
from luojianet_ms.ops import operations as P
from ....train_step_wrap import train_step_with_loss_warp, train_step_with_sens

context.set_context(mode=context.GRAPH_MODE)


class LeNet5(nn.Module):
    """LeNet5 definition"""

    def __init__(self):
        super(LeNet5, self).__init__()
        self.conv1 = nn.Conv2d(1, 6, 5, pad_mode='valid')
        self.conv2 = nn.Conv2d(6, 16, 5, pad_mode='valid')
        self.fc1 = nn.Dense(16 * 5 * 5, 120)
        self.fc2 = nn.Dense(120, 84)
        self.fc3 = nn.Dense(84, 10)
        self.relu = nn.ReLU()
        self.max_pool2d = nn.MaxPool2d(kernel_size=2, stride=2)
        self.flatten = P.Flatten()

    def call(self, x):
        x = self.max_pool2d(self.relu(self.conv1(x)))
        x = self.max_pool2d(self.relu(self.conv2(x)))
        x = self.flatten(x)
        x = self.relu(self.fc1(x))
        x = self.relu(self.fc2(x))
        x = self.fc3(x)
        return x


def test_lenet5_train_step():
    predict = Tensor(np.ones([1, 1, 32, 32]).astype(np.float32) * 0.01)
    label = Tensor(np.zeros([1, 10]).astype(np.float32))
    net = train_step_with_loss_warp(LeNet5())
    _cell_graph_executor.compile(net, predict, label)


def test_lenet5_train_sens():
    predict = Tensor(np.ones([1, 1, 32, 32]).astype(np.float32) * 0.01)
    sens = Tensor(np.ones([1, 10]).astype(np.float32))
    net = train_step_with_sens(LeNet5(), sens)
    _cell_graph_executor.compile(net, predict)


def test_lenet5_train_step_training():
    predict = Tensor(np.ones([1, 1, 32, 32]).astype(np.float32) * 0.01)
    label = Tensor(np.zeros([1, 10]).astype(np.float32))
    net = train_step_with_loss_warp(LeNet5())
    net.set_train()
    _cell_graph_executor.compile(net, predict, label)
