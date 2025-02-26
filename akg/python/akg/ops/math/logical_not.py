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

"""operator dsl function: logical_not"""
import akg.tvm
import akg.topi
import akg.utils as utils

@utils.check_input_type(akg.tvm.tensor.Tensor, (str, type(None)))
def LogicalNot(input1, target=utils.CCE):
    """
    Compute logical_not of input1.

    Args:
        input1 (tvm.tensor.Tensor): Tensor.

    Returns:
        tvm.tensor.Tensor.
    
    Supported Platforms:
        'Ascend', 'GPU', 'CPU'
    """
    utils.check_supported_target(target)
    res = akg.topi.logical_not(input1)
    return res
