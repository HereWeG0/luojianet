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
""" test syntax for logic expression """

import numpy as np

import luojianet_ms.nn as nn
import luojianet_ms
from luojianet_ms import context
from luojianet_ms.common.tensor import Tensor
from luojianet_ms.ops import operations as P

context.set_context(mode=context.GRAPH_MODE)


class ArgumentNum(nn.Module):
    def __init__(self):
        super().__init__()
        self.matmul = P.MatMul()

    def call(self, x, y):
        super(ArgumentNum, 2, 3).aa()
        out = self.matmul(x, y)
        return out


def test_super_argument_num():
    x = Tensor(np.ones(shape=[1, 3]), luojianet_ms.float32)
    y = Tensor(np.ones(shape=[3, 4]), luojianet_ms.float32)
    net = ArgumentNum()
    ret = net(x, y)
    print(ret)


class ArgumentNotSelf(nn.Module):
    def __init__(self):
        super().__init__()
        self.matmul = P.MatMul()

    def call(self, x, y):
        super(ArgumentNotSelf, 2).aa()
        out = self.matmul(x, y)
        return out


def test_super_argument_not_self():
    x = Tensor(np.ones(shape=[1, 3]), luojianet_ms.float32)
    y = Tensor(np.ones(shape=[3, 4]), luojianet_ms.float32)
    net = ArgumentNotSelf()
    ret = net(x, y)
    print(ret)


class ArgumentType(nn.Module):
    def __init__(self):
        super().__init__()
        self.matmul = P.MatMul()

    def call(self, x, y):
        super(ArgumentType, self).aa()
        out = self.matmul(x, y)
        return out


def test_super_argument_type():
    x = Tensor(np.ones(shape=[1, 3]), luojianet_ms.float32)
    y = Tensor(np.ones(shape=[3, 4]), luojianet_ms.float32)
    net = ArgumentType()
    ret = net(x, y)
    print(ret)
