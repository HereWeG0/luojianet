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
import luojianet_ms.common.dtype as mstype
from luojianet_ms import Tensor
from luojianet_ms.ops import operations as P

context.set_context(mode=context.GRAPH_MODE,
                    device_target="Ascend")


class Net(nn.Module):
    def __init__(self, offset):
        super(Net, self).__init__()
        self.embedding = P.EmbeddingLookup()
        self.offset = offset

    def call(self, param, index):
        return self.embedding(param, index, self.offset)


def test_embedding_lookup_sparse():
    params = Tensor(np.array([[8, 9], [10, 11], [12, 13], [14, 15]]), mstype.int32)
    indices = Tensor(np.array([[5, 2], [8, 5]]), mstype.int32)
    offset = 4
    embedding = Net(offset)
    out = embedding(params, indices)
    assert(out.asnumpy() == [[[10, 11], [0, 0]], [[0, 0], [10, 11]]]).all()
