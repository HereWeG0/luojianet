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
import pytest

import luojianet_ms.context as context
import luojianet_ms.nn as nn
from luojianet_ms import Tensor
from luojianet_ms.ops import operations as P

context.set_context(mode=context.GRAPH_MODE, device_target='CPU')


class OpNetWrapper(nn.Module):
    def __init__(self, op):
        super(OpNetWrapper, self).__init__()
        self.op = op

    def call(self, *inputs):
        return self.op(*inputs)


@pytest.mark.level0
@pytest.mark.platform_x86_cpu
@pytest.mark.env_onecard
def test_notequal_int():
    op = P.NotEqual()
    op_wrapper = OpNetWrapper(op)

    input_x = Tensor(np.array([1, 2, 3]).astype(np.int32))
    input_y = Tensor(np.array([11, 2, 13]).astype(np.int32))
    outputs = op_wrapper(input_x, input_y)

    print(outputs)
    assert np.allclose(outputs.asnumpy(), (True, False, True))


@pytest.mark.level0
@pytest.mark.platform_x86_cpu
@pytest.mark.env_onecard
def test_notequal_float():
    op = P.NotEqual()
    op_wrapper = OpNetWrapper(op)

    input_x = Tensor(np.array([1, 2, 3]).astype(np.float32))
    input_y = Tensor(np.array([-1, 0, 3]).astype(np.float32))
    outputs = op_wrapper(input_x, input_y)

    print(outputs)
    assert np.allclose(outputs.asnumpy(), (True, True, False))


if __name__ == '__main__':
    test_notequal_int()
    test_notequal_float()
