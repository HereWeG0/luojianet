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
import luojianet_ms
import luojianet_ms.context as context
import luojianet_ms.nn as nn
from luojianet_ms import Tensor
from luojianet_ms.ops import operations as P

context.set_context(mode=context.GRAPH_MODE, device_target="Ascend")


class Net(nn.Module):
    def __init__(self, right=False, out_int32=True):
        super(Net, self).__init__()
        self.search = P.SearchSorted(out_int32=out_int32, right=right)

    def call(self, sequence, values):
        return self.search(sequence, values)


def test_net_int32():
    np.random.seed(1)
    input1 = np.sort(np.array(np.random.randint(10, size=(2, 3, 9)), dtype=np.int32), axis=-1)
    sequence = Tensor(input1, luojianet_ms.int32)
    input2 = np.array(np.random.randint(10, size=(2, 3, 1)), dtype=np.int32)
    values = Tensor(input2, luojianet_ms.int32)
    net = Net()
    output = net(sequence, values)
    print(output)
