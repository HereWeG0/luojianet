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
import luojianet_ms.nn as nn
import luojianet_ms.context as context
from luojianet_ms import Tensor
from luojianet_ms.ops import operations as P

context.set_context(mode=context.GRAPH_MODE, device_target="Ascend")
class Net(nn.Module):
    def __init__(self, num_sample):
        super(Net, self).__init__()
        self.random_categorical = P.RandomCategorical(luojianet_ms.int64)
        self.num_sample = num_sample

    def call(self, logits, seed=0):
        return self.random_categorical(logits, self.num_sample, seed)

def test_net():
    x = np.random.random((10, 5)).astype(np.float32)
    net = Net(8)
    output = net(Tensor(x))
    print(x)
    print(output.asnumpy())
    #print(output.dtype())
