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

import luojianet_ms.context as context
import luojianet_ms.nn as nn
from luojianet_ms import Tensor
from luojianet_ms.common.initializer import initializer
from luojianet_ms.common.parameter import Parameter
from luojianet_ms.ops import operations as P


class Net(nn.Module):
    def __init__(self):
        super(Net, self).__init__()
        self.bn = P.BatchNorm()
        self.scale = Parameter(initializer('ones', [64]), name='scale')
        self.offset = Parameter(initializer('zeros', [64]), name='offset')
        self.mean = Parameter(initializer('ones', [64]), name='mean')
        self.variance = Parameter(initializer('zeros', [64]), name='variance')

    def call(self, x):
        return self.bn(x, self.scale, self.offset, self.mean, self.variance)[0]


def test_net():
    x = np.random.randn(1, 64, 112, 112).astype(np.float32)
    # mean = np.random.randn(1,16,1,1).astype(np.float32)
    # variance = np.random.randn(1,16,1,1).astype(np.float32)
    context.set_context(mode=context.GRAPH_MODE, device_target="Ascend")
    Bn = Net()
    output = Bn(Tensor(x))
    print("***********x*********")
    print(x)

    print("***********output y*********")
    print(output.asnumpy())

    context.set_context(mode=context.PYNATIVE_MODE, device_target="Ascend")
    Bn = Net()
    output = Bn(Tensor(x))
    print("***********x*********")
    print(x)

    print("***********output y*********")
    print(output.asnumpy())
