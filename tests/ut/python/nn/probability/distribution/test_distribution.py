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
"""
Test nn.probability.distribution.
"""
import pytest

import luojianet_ms.nn as nn
import luojianet_ms.nn.probability.distribution as msd
from luojianet_ms import dtype as mstype
from luojianet_ms import Tensor
from luojianet_ms import context

func_name_list = ['prob', 'log_prob', 'cdf', 'log_cdf',
                  'survival_function', 'log_survival',
                  'sd', 'var', 'mode', 'mean',
                  'entropy', 'kl_loss', 'cross_entropy',
                  'sample']


class MyExponential(msd.Distribution):
    """
    Test distribution class: no function is implemented.
    """

    def __init__(self, rate=None, seed=None, dtype=mstype.float32, name="MyExponential"):
        param = dict(locals())
        param['param_dict'] = {'rate': rate}
        super(MyExponential, self).__init__(seed, dtype, name, param)


class Net(nn.Module):
    """
    Test Net: function called through call.
    """

    def __init__(self, func_name):
        super(Net, self).__init__()
        self.dist = MyExponential()
        self.name = func_name

    def call(self, *args, **kwargs):
        return self.dist(self.name, *args, **kwargs)


def test_raise_not_implemented_error_construct():
    """
    test raise not implemented error in pynative mode.
    """
    value = Tensor([0.2], dtype=mstype.float32)
    for func_name in func_name_list:
        with pytest.raises(NotImplementedError):
            net = Net(func_name)
            net(value)


def test_raise_not_implemented_error_construct_graph_mode():
    """
    test raise not implemented error in graph mode.
    """
    context.set_context(mode=context.GRAPH_MODE)
    value = Tensor([0.2], dtype=mstype.float32)
    for func_name in func_name_list:
        with pytest.raises(NotImplementedError):
            net = Net(func_name)
            net(value)


class Net1(nn.Module):
    """
    Test Net: function called directly.
    """

    def __init__(self, func_name):
        super(Net1, self).__init__()
        self.dist = MyExponential()
        self.func = getattr(self.dist, func_name)

    def call(self, *args, **kwargs):
        return self.func(*args, **kwargs)


def test_raise_not_implemented_error():
    """
    test raise not implemented error in pynative mode.
    """
    value = Tensor([0.2], dtype=mstype.float32)
    for func_name in func_name_list:
        with pytest.raises(NotImplementedError):
            net = Net1(func_name)
            net(value)


def test_raise_not_implemented_error_graph_mode():
    """
    test raise not implemented error in graph mode.
    """
    context.set_context(mode=context.GRAPH_MODE)
    value = Tensor([0.2], dtype=mstype.float32)
    for func_name in func_name_list:
        with pytest.raises(NotImplementedError):
            net = Net1(func_name)
            net(value)
