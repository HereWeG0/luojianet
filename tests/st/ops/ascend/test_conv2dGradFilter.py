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
from luojianet_ms.common.parameter import Parameter
from luojianet_ms.ops import operations as P
from luojianet_ms.ops.operations import _grad_ops as G

context.set_context(device_target="Ascend")


class Net(nn.Module):
    def __init__(self):
        super(Net, self).__init__()
        self.conv2d_grad = G.Conv2DBackpropFilter(4, 1)
        yt = Tensor(np.array([[[[1, 0, -1], [1, 0, -1], [1, 0, -1]]]]).astype(np.float32))
        self.y = Parameter(yt, name='y')
        self.get_shape = P.Shape()

    @ms_function
    def call(self, x_, out_):
        return self.conv2d_grad(out_, x_, self.get_shape(self.y))


x = Tensor(np.array([[[
    [3, 0, 1, 2, 7, 4],
    [1, 5, 8, 9, 3, 1],
    [2, 7, 2, 5, 1, 3],
    [0, 1, 3, 1, 7, 8],
    [4, 2, 1, 6, 2, 8],
    [2, 4, 5, 2, 3, 9]]]]).astype(np.float32))

out = Tensor(np.array([[[
    [-5, -4, 0, 8],
    [-10, -2, 2, 3],
    [0, -2, -4, -7],
    [-3, -2, -3, -16]]]]).astype(np.float32))

operator = Net()
output = operator(x, out)
expect_out = np.array([[[[-60., -142., -265.], [-104., -211., -322.], [-102., -144., -248.]]]]).astype(np.float32)
print(output.asnumpy())
print(expect_out)
assert np.all(output.asnumpy() == expect_out), "conv2d_grad execute failed, please check current code commit"
