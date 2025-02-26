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
from luojianet_ms.common.api import ms_function
from luojianet_ms.ops import operations as P
from luojianet_ms.ops.composite import GradOperation

context.set_context(device_target="Ascend")


class Grad(nn.Module):
    def __init__(self, network):
        super(Grad, self).__init__()
        self.grad = GradOperation(get_all=True, sens_param=True)
        self.network = network

    @ms_function
    def call(self, input_, output_grad):
        return self.grad(self.network)(input_, output_grad)


class Net(nn.Module):
    def __init__(self):
        super(Net, self).__init__()
        self.maxpool = P.MaxPool(pad_mode="SAME", window=3, stride=2)

    @ms_function
    def call(self, x):
        output = self.maxpool(x)
        return output


def test_net():
    x = np.random.randn(32, 64, 112, 112).astype(np.float32)
    output_grad = np.random.randn(32, 64, 56, 56).astype(np.float32)
    net = Grad(Net())
    output = net(Tensor(x), Tensor(output_grad))
    if isinstance(output, (tuple, list)):
        output = output[0]
    print(output.asnumpy())
