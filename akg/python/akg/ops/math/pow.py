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

"""operator dsl function: pow"""
import akg.topi
import akg.tvm
import akg.utils as utils
from .cast import Cast
from akg.utils.dsl_create import produce_shapes
from akg.utils.kernel_exec import product_is_mini

def _pow_ascend(data, scale, target):
    shape1 = [x.value for x in data.shape]
    shape2 = [x.value for x in scale.shape]

    check_list = ["float16", "float32", "int32", "int8", "uint8"]
    dtype = data.dtype
    if not (dtype.lower() in check_list):
        raise RuntimeError("tile_cce only support %s while dtype is %s" % (",".join(check_list), dtype))

    shape = [x.value for x in data.shape]
    utils.check_shape(shape)
    utils.auto_broadcast_check(shape1, shape2)
    compute_dtype = "float32"
    if product_is_mini():
        compute_dtype = "float16" 
    data = Cast(data, compute_dtype, target)
    scale = Cast(scale, compute_dtype, target)

    C = akg.topi.power(data, scale)
    C = Cast(C, dtype, target)
    return C

@utils.check_input_type(akg.tvm.tensor.Tensor, akg.tvm.tensor.Tensor)
def _pow(data1, data2):
    utils.elemwise_dtype_check(data1.dtype, data2.dtype)
    utils.check_shape(data1.shape)
    utils.check_shape(data2.shape)
    utils.auto_broadcast_check(data1.shape, data2.shape)

    in_dtype = data1.dtype
    if in_dtype == 'float16':
        data1 = akg.topi.cast(data1, 'float32')
        data2 = akg.topi.cast(data2, 'float32')
    res = akg.topi.power(data1, data2)
    if in_dtype == 'float16':
        res = akg.topi.cast(res, 'float16')

    return res

def Pow(data1, data2, target=utils.CCE):
    """
    Computes power(data1,data2) elementwise, broadcast is supported.

    Args:
        data1 (tvm.tensor.Tensor): Tensor.
        data2 (tvm.tensor.Tensor): Tensor of same type as data1, if shape(data2) != shape(data1), broadcast will happen.

    Returns:
        tvm.tensor.Tensor, powered result, with same type as input tensors and broadcasted shape of data1 and data2.
    
    Supported Platforms:
        'Ascend', 'GPU', 'CPU'
    """
    utils.check_supported_target(target)
    if target == utils.CCE:
        return _pow_ascend(data1, data2, target)
    else:
        return _pow(data1, data2)