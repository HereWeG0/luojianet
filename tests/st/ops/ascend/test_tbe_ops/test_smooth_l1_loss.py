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
from luojianet_ms.ops import operations as P

context.set_context(mode=context.GRAPH_MODE, device_target="Ascend")


class Net(nn.Module):
    def __init__(self, sigma=1.0):
        super(Net, self).__init__()
        self.SmoothL1Loss = P.SmoothL1Loss(sigma)

    def call(self, pred, gt):
        return self.SmoothL1Loss(pred, gt)


def test_net():
    pred = np.random.randn(2, 4).astype(np.float32)
    gt = np.random.randn(2, 4).astype(np.float32)
    smooth_l1_loss = Net()
    loss = smooth_l1_loss(Tensor(pred), Tensor(gt))
    print("------------- input ---------------")
    print("predict:\n", pred)
    print("grount truth:\n", gt)
    print("------------- output ---------------")
    print("loss:\n", loss.asnumpy())
