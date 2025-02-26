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
"""Test dump."""
import warnings

import pytest

import luojianet_ms.context as context
import luojianet_ms.nn as nn
import luojianet_ms.ops as ops
from luojianet_ms import set_dump


def test_set_dump_on_cell():
    """
    Feature: Python API set_dump.
    Description: Use set_dump API on Module instance.
    Expectation: Success.
    """

    class MyNet(nn.Module):
        def __init__(self):
            super(MyNet, self).__init__()
            self.conv1 = nn.Conv2d(5, 6, 5, pad_mode='valid')
            self.relu1 = nn.ReLU()

        def call(self, x):
            x = self.conv1(x)
            x = self.relu1(x)
            return x

    net = MyNet()
    set_dump(net.relu1)

    assert net.relu1.relu.attrs["dump"] == "true"


def test_set_dump_on_primitive():
    """
    Feature: Python API set_dump.
    Description: Use set_dump API on Primitive instance.
    Expectation: Success.
    """
    op = ops.Add()
    set_dump(op)
    assert op.attrs["dump"] == "true"


def test_input_type_check():
    """
    Feature: Python API set_dump.
    Description: Use set_dump API on unsupported instance.
    Expectation: Throw ValueError exception.
    """
    with pytest.raises(ValueError):
        set_dump(1)


@pytest.mark.skip(reason="Warning can only be triggered once, please execute "
                         "this test case manually.")
def test_set_dump_warning():
    """
    Feature: Python API set_dump.
    Description: Test the warning about device target and mode.
    Expectation: Trigger warning message.
    """
    context.set_context(device_target="CPU")
    context.set_context(mode=context.PYNATIVE_MODE)
    op = ops.Add()
    with warnings.catch_warnings(record=True) as w:
        warnings.simplefilter("always")
        set_dump(op)
        assert "Only Ascend device target is supported" in str(w[-2].message)
        assert "Only GRAPH_MODE is supported" in str(w[-1].message)
