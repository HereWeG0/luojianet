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
""" test_cont_break """
import numpy as np

from luojianet_ms import Tensor, Model, context
from luojianet_ms.nn import Module
from ...ut_filter import non_graph_engine


def run_test(netclass, count):
    context.set_context(mode=context.GRAPH_MODE)
    net = netclass()
    model = Model(net)
    for _ in range(count):
        input_np = np.random.randn(2, 3).astype(np.float32)
        input_ms = Tensor(input_np)
        output_np = net.call(input_np)  # run python
        output_ms = model.predict(input_ms)  # run graph
        assert np.shape(output_np) == np.shape(output_ms.asnumpy())
        # Disable equal assert because UT in CI use fake backend.
        # np.testing.assert_array_almost_equal(output_np, output_ms.asnumpy(), decimal=3)


# pylint: disable=unnecessary-pass
class for_loop_with_break(Module):
    def __init__(self):
        super().__init__()

    def call(self, x):
        for i in range(8):
            if i > 5:
                x *= 3
                break
            x = x * 2
            pass
        return x


@non_graph_engine
def test_for_loop_with_break():
    run_test(for_loop_with_break, 10)


class for_loop_with_continue(Module):
    def __init__(self):
        super().__init__()

    def call(self, x):
        for i in range(8):
            if i > 5:
                x *= 3
                continue
            x = x * 2
        return x


@non_graph_engine
def test_for_loop_with_continue():
    run_test(for_loop_with_continue, 10)

# pylint: disable=unnecessary-pass
class for_loop_with_cont_break(Module):
    def __init__(self):
        super().__init__()

    def call(self, x):
        for i in range(8):
            if i < 3:
                i *= 2
                continue
            if i > 5:
                x *= 3
                break
                # x *= 2
            x = x * 2
            pass
        return x


@non_graph_engine
def test_for_loop_with_cont_break():
    run_test(for_loop_with_cont_break, 10)


class for_nested_loop_with_break(Module):
    def __init__(self):
        super().__init__()

    def call(self, x):
        for i in range(3):
            for j in range(5):
                if j > 3:
                    x *= 2
                    break
                x = x * 1.5
        return x


@non_graph_engine
def test_for_nested_loop_with_break():
    run_test(for_nested_loop_with_break, 10)


class while_with_break(Module):
    def __init__(self):
        super().__init__()

    def call(self, x):
        i = 0
        while i < 5:
            if i > 3:
                x *= 2
                break
            x = x * 1.5
            i += 1
        return x


@non_graph_engine
def test_while_with_break():
    run_test(while_with_break, 10)


class while_with_continue(Module):
    def __init__(self):
        super().__init__()

    def call(self, x):
        i = 0
        while i < 5:
            if i > 3:
                x *= 2
                i += 1
                continue
            x = x * 1.5
            i += 1
        return x


@non_graph_engine
def test_while_with_continue():
    run_test(while_with_continue, 10)


class while_for_nested(Module):
    def __init__(self):
        super().__init__()

    def call(self, x):
        i = 0
        while i < 5:
            if i > 3:
                for j in range(3):
                    if j > 1:
                        break
                    x *= 2
                i += 1
                continue
            x = x * 1.5
            i += 1
        return x


@non_graph_engine
def test_while_for_nested():
    run_test(while_for_nested, 10)


class pass_branch(Module):
    def __init__(self):
        super().__init__()

    def call(self, x):
        i = 0
        while i < 5:
            if i > 3:
                pass
            else:
                x = x * 1.5
            i += 1
        return x


@non_graph_engine
def test_pass_branch():
    run_test(pass_branch, 10)
