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
""" test list mul number """

import numpy as np
from luojianet_ms import Tensor, context
from luojianet_ms import nn


class Net(nn.Module):
    def __init__(self):
        super(Net, self).__init__()
        self.list_ = [Tensor([1, 2, 3])]
        self.number1 = 5
        self.number2 = 0

    def call(self):
        return self.list_ * self.number1, self.list_ * self.number2

def test_list_mul_number():
    """
    Description: test_list_mul_number
    Expectation: the results are as expected
    """

    context.set_context(mode=context.GRAPH_MODE)
    net = Net()
    expect_ret0 = [Tensor([1, 2, 3])] * 5
    expect_ret1 = [Tensor([1, 2, 3])] * 0
    assert isinstance(net()[0], list)
    assert isinstance(net()[1], list)
    for i in range(len(net()[0])):
        assert np.array_equal(net()[0][i].asnumpy(), expect_ret0[i].asnumpy())
    assert net()[1] == expect_ret1
