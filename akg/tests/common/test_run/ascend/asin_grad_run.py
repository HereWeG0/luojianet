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

"""run function for asin_grad"""

import numpy as np

from akg.utils import kernel_exec as utils
from tests.common.base import get_rtol_atol
from tests.common.gen_random import random_gaussian
from tests.common.tensorio import compare_tensor
from akg.ops.math.ascend import AsinGrad

def asin_grad_run(shape, dtype, attrs=None):
    mod = utils.op_build_test(AsinGrad, [shape, shape], [dtype, dtype],
                              kernel_name="asin_grad", attrs=attrs)
    expect, inputs, output = gen_data(dtype, shape)
    output = utils.mod_launch(mod, (*inputs, output), expect=expect)
    rtol, atol = get_rtol_atol("asin_grad", dtype)
    TestCase_Result = compare_tensor(output, expect, rtol=rtol, atol=atol, equal_nan=False)

    return inputs, output, expect, TestCase_Result


def gen_data(dtype, shape):
    x = (np.random.rand(*shape) * 2 - 1).astype(dtype)
    dy = random_gaussian(shape, miu=0, sigma=1).astype(dtype)
    inputs = [x, dy]
    x = x.astype("float32")
    dy = dy.astype("float32")
    one = np.array([1]).astype(dtype)
    expect = dy * (one / np.sqrt(one - x * x))
    expect = expect.astype(dtype)
    output = np.full(shape, np.nan, dtype)
    return expect, inputs, output
