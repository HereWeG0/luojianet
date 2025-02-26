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
""" test_tensor_slice """
import numpy as np
import pytest

from luojianet_ms import Tensor
from luojianet_ms import context
from luojianet_ms import dtype as mstype
from luojianet_ms.nn import Module
from ....luojianet_ms_test_framework.luojianet_ms_test import luojianet_ms_test
from ....luojianet_ms_test_framework.pipeline.forward.compile_forward \
    import pipeline_for_compile_forward_ge_graph_for_case_by_case_config, \
    pipeline_for_compile_forward_ge_graph_for_case_by_case_config_exception


class NetWorkFancyIndex(Module):
    def __init__(self, index):
        super(NetWorkFancyIndex, self).__init__()
        self.index = index

    def call(self, tensor):
        return tensor[self.index]


class TensorItemByNone(Module):
    def call(self, tensor):
        ret = tensor.item()
        return ret


class TensorItemByItem(Module):
    def call(self, tensor, index):
        ret = tensor.item(index)
        return ret


def test_tensor_fancy_index_integer_list():
    context.set_context(mode=context.GRAPH_MODE)
    index = [0, 2, 1]
    net = NetWorkFancyIndex(index)
    input_np = np.arange(60).reshape(3, 4, 5)
    input_me = Tensor(input_np, dtype=mstype.float32)
    net(input_me)


def test_tensor_fancy_index_boolean_list():
    context.set_context(mode=context.GRAPH_MODE)
    index = [True, True, False]
    net = NetWorkFancyIndex(index)
    input_np = np.arange(60).reshape(3, 4, 5)
    input_me = Tensor(input_np, dtype=mstype.float32)
    net(input_me)


def test_tensor_fancy_index_integer_boolean_list_graph():
    context.set_context(mode=context.GRAPH_MODE)
    index = [1, 2, True, False]
    net = NetWorkFancyIndex(index)
    input_np = np.arange(60).reshape(3, 4, 5)
    input_me = Tensor(input_np, dtype=mstype.float32)
    net(input_me)


def test_tensor_fancy_index_integer_list_mixed():
    context.set_context(mode=context.GRAPH_MODE)
    index = (1, [2, 1, 3], slice(1, 3, 1), ..., 4)
    net = NetWorkFancyIndex(index)
    input_np = np.arange(3*4*5*6*7*8).reshape(3, 4, 5, 6, 7, 8)
    input_me = Tensor(input_np, dtype=mstype.float32)
    net(input_me)


def test_tensor_fancy_index_integer_tuple_mixed():
    context.set_context(mode=context.GRAPH_MODE)
    index = (1, (2, 1, 3), slice(1, 3, 1), ..., 4)
    net = NetWorkFancyIndex(index)
    input_np = np.arange(3*4*5*6*7*8).reshape(3, 4, 5, 6, 7, 8)
    input_me = Tensor(input_np, dtype=mstype.float32)
    net(input_me)


def test_tensor_fancy_index_integer_list_tuple_mixed():
    context.set_context(mode=context.GRAPH_MODE)
    index = (1, [2, 1, 3], (3, 2, 1), slice(1, 3, 1), ..., 4)
    net = NetWorkFancyIndex(index)
    input_np = np.arange(3*4*5*6*7*8).reshape(3, 4, 5, 6, 7, 8)
    input_me = Tensor(input_np, dtype=mstype.float32)
    net(input_me)


def test_tensor_fancy_index_integer_list_tuple_bool_mixed():
    context.set_context(mode=context.GRAPH_MODE)
    index = (1, [2, 1, 3], True, (3, 2, 1), slice(1, 3, 1), ..., True, 4)
    net = NetWorkFancyIndex(index)
    input_np = np.arange(3*4*5*6*7*8).reshape(3, 4, 5, 6, 7, 8)
    input_me = Tensor(input_np, dtype=mstype.float32)
    net(input_me)


def test_tensor_fancy_index_integer_list_tuple_bool_mixed_error():
    context.set_context(mode=context.GRAPH_MODE)
    index = (1, [2, 1, 3], True, (3, 2, 1), slice(1, 3, 1), ..., False, 4)
    net = NetWorkFancyIndex(index)
    input_np = np.arange(3*4*5*6*7*8).reshape(3, 4, 5, 6, 7, 8)
    input_me = Tensor(input_np, dtype=mstype.float32)
    with pytest.raises(IndexError):
        net(input_me)


input_1d_np = np.ndarray([1]).astype(np.float32)
input_1d_ms = Tensor(input_1d_np, mstype.float32)
input_3d_np = np.random.randint(3, size=(3, 4, 5)).astype(np.int32)
input_3d_ms = Tensor(input_3d_np, mstype.float32)
index_np_1, index_np_2, index_np_3, index_np_4 = 0, 1.0, 30, 60
tuple_index_np_1, tuple_index_np_2, tuple_index_np_3, tuple_index_np_4, tuple_index_np_5 = \
    (0,), (1, 2), (1, 2, 3), (3, 4, 4), (1, 2, 3, 4)

test_cases = [
    ('TensorItemByNone', {'block': TensorItemByNone(), 'desc_inputs': [input_1d_ms],}),
    ('1dTensorItemByInt', {'block': TensorItemByItem(), 'desc_inputs': [input_1d_ms, index_np_1],}),
    ('3dTensorItemByInt', {'block': TensorItemByItem(), 'desc_inputs': [input_3d_ms, index_np_1],}),
    ('3dTensorItemByInt2', {'block': TensorItemByItem(), 'desc_inputs': [input_3d_ms, index_np_3],}),
    ('1dTensorItemByTuple', {'block': TensorItemByItem(), 'desc_inputs': [input_1d_ms, tuple_index_np_1],}),
    ('3dTensorItemByTuple', {'block': TensorItemByItem(), 'desc_inputs': [input_3d_ms, tuple_index_np_3],}),
]


test_error_cases = [
    ('TensorItemByNoneForMulDimsTensor', {
        'block': (TensorItemByNone(), {'exception': ValueError}),
        'desc_inputs': [input_3d_ms]
    }),
    ('TensorItemByFloatError', {
        'block': (TensorItemByItem(), {'exception': TypeError}),
        'desc_inputs': [input_1d_ms, index_np_2]
    }),
    ('TensorItemByFloatError2', {
        'block': (TensorItemByItem(), {'exception': TypeError}),
        'desc_inputs': [input_3d_ms, index_np_2]
    }),
    ('TensorItemByIntOverBoundary', {
        'block': (TensorItemByItem(), {'exception': IndexError}),
        'desc_inputs': [input_1d_ms, index_np_3]
    }),
    ('TensorItemByIntOverBoundary2', {
        'block': (TensorItemByItem(), {'exception': IndexError}),
        'desc_inputs': [input_3d_ms, index_np_4]
    }),
    ('1dTensorItemBy2dTuple', {
        'block': (TensorItemByItem(), {'exception': ValueError}),
        'desc_inputs': [input_1d_ms, tuple_index_np_2]
    }),
    ('3dTensorItemBy2dTuple', {
        'block': (TensorItemByItem(), {'exception': ValueError}),
        'desc_inputs': [input_3d_ms, tuple_index_np_2]
    }),
    ('3dTensorItemBy3dTupleOutOfBoundary', {
        'block': (TensorItemByItem(), {'exception': IndexError}),
        'desc_inputs': [input_3d_ms, tuple_index_np_4]
    }),
    ('3dTensorItemBy4dTuple', {
        'block': (TensorItemByItem(), {'exception': ValueError}),
        'desc_inputs': [input_3d_ms, tuple_index_np_5]
    })
]


@luojianet_ms_test(pipeline_for_compile_forward_ge_graph_for_case_by_case_config)
def test_exec():
    context.set_context(mode=context.GRAPH_MODE)
    return test_cases


@luojianet_ms_test(pipeline_for_compile_forward_ge_graph_for_case_by_case_config_exception)
def test_check_exception():
    return test_error_cases
