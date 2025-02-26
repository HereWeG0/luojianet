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

"""NonZero op"""
from luojianet_ms.ops.op_info_register import op_info_register, TBERegOp, DataType

non_zero_op_info = TBERegOp("NonZero") \
    .fusion_type("OPAQUE") \
    .async_flag(False) \
    .binfile_name("non_zero.so") \
    .compute_cost(10) \
    .kernel_name("non_zero") \
    .partial_flag(True) \
    .dynamic_shape(True) \
    .attr("transpose", "optional", "bool", "all", "false") \
    .input(0, "x", False, "required", "all") \
    .output(0, "y", False, "required", "all") \
    .dtype_format(DataType.F32_Default, DataType.I64_Default) \
    .get_op_info()


@op_info_register(non_zero_op_info)
def _non_zero_ds_tbe():
    """NonZero TBE register"""
    return
