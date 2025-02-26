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

context.set_context(mode=context.PYNATIVE_MODE, device_target="Ascend")


class Net(nn.Module):
    def __init__(self):
        super(Net, self).__init__()
        self.isfinite = P.IsFinite()

    def call(self, tensor):
        return self.isfinite(tensor)


def test_net_bool():
    x = np.random.randn(1, 16, 1, 1).astype(np.bool)
    net = Net()
    output = net(Tensor(x))
    print(output.asnumpy())
    assert np.all(output.asnumpy() == np.isfinite(x))


def test_net_int8():
    x = np.random.randn(1, 16, 1, 1).astype(np.int8)
    net = Net()
    output = net(Tensor(x))
    print(output.asnumpy())
    assert np.all(output.asnumpy() == np.isfinite(x))


def test_net_uint8():
    x = np.random.randn(1, 16, 1, 1).astype(np.uint8)
    net = Net()
    output = net(Tensor(x))
    print(output.asnumpy())
    assert np.all(output.asnumpy() == np.isfinite(x))


def test_net_int16():
    x = np.random.randn(1, 16, 1, 1).astype(np.int16)
    net = Net()
    output = net(Tensor(x))
    print(output.asnumpy())
    assert np.all(output.asnumpy() == np.isfinite(x))


def test_net_uint16():
    x = np.random.randn(1, 16, 1, 1).astype(np.uint16)
    net = Net()
    output = net(Tensor(x))
    print(output.asnumpy())
    assert np.all(output.asnumpy() == np.isfinite(x))


def test_net_int32():
    x = np.random.randn(1, 16, 1, 1).astype(np.int32)
    net = Net()
    output = net(Tensor(x))
    print(output.asnumpy())
    assert np.all(output.asnumpy() == np.isfinite(x))


def test_net_uint32():
    x = np.random.randn(1, 16, 1, 1).astype(np.uint32)
    net = Net()
    output = net(Tensor(x))
    print(output.asnumpy())
    assert np.all(output.asnumpy() == np.isfinite(x))


def test_net_int64():
    x = np.random.randn(1, 16, 1, 1).astype(np.int64)
    net = Net()
    output = net(Tensor(x))
    print(output.asnumpy())
    assert np.all(output.asnumpy() == np.isfinite(x))


def test_net_uint64():
    x = np.random.randn(1, 16, 1, 1).astype(np.uint64)
    net = Net()
    output = net(Tensor(x))
    print(output.asnumpy())
    assert np.all(output.asnumpy() == np.isfinite(x))


def test_net_float16():
    x = np.random.randn(1, 16, 1, 1).astype(np.float16)
    net = Net()
    output = net(Tensor(x))
    print(output.asnumpy())
    assert np.all(output.asnumpy() == np.isfinite(x))


def test_net_float32():
    x = np.random.randn(1, 16, 1, 1).astype(np.float32)
    net = Net()
    output = net(Tensor(x))
    print(output.asnumpy())
    assert np.all(output.asnumpy() == np.isfinite(x))


def test_net_float64():
    x = np.random.randn(1, 16, 1, 1).astype(np.float64)
    net = Net()
    output = net(Tensor(x))
    print(output.asnumpy())
    assert np.all(output.asnumpy() == np.isfinite(x))
