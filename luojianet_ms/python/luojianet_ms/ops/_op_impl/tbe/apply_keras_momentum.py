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

"""ApplyKerasMomentum op"""
from luojianet_ms.ops.op_info_register import op_info_register, TBERegOp, DataType

apply_keras_momentum_op_info = TBERegOp("ApplyKerasMomentum") \
    .fusion_type("OPAQUE") \
    .async_flag(False) \
    .binfile_name("apply_keras_momentum_d.so") \
    .compute_cost(10) \
    .kernel_name("apply_keras_momentum_d") \
    .partial_flag(True) \
    .attr("use_locking", "optional", "bool", "true,false", "false") \
    .attr("use_nesterov", "optional", "bool", "true,false", "false") \
    .input(0, "var", False, "required", "all") \
    .input(1, "accum", False, "required", "all") \
    .input(2, "lr", False, "required", "all") \
    .input(3, "grad", False, "required", "all") \
    .input(4, "momentum", False, "required", "all") \
    .output(0, "var", False, "required", "all") \
    .output(1, "accum", False, "required", "all") \
    .dtype_format(DataType.F32_5HD, DataType.F32_5HD, DataType.F32_Default, DataType.F32_5HD,
                  DataType.F32_Default, DataType.F32_5HD, DataType.F32_5HD) \
    .dtype_format(DataType.F32_C1HWNCoC0, DataType.F32_C1HWNCoC0, DataType.F32_Default, DataType.F32_C1HWNCoC0,
                  DataType.F32_Default, DataType.F32_C1HWNCoC0, DataType.F32_C1HWNCoC0) \
    .dtype_format(DataType.F32_Default, DataType.F32_Default, DataType.F32_Default, DataType.F32_Default,
                  DataType.F32_Default, DataType.F32_Default, DataType.F32_Default) \
    .dtype_format(DataType.F32_FracZ, DataType.F32_FracZ, DataType.F32_Default, DataType.F32_Default,
                  DataType.F32_Default, DataType.F32_FracZ, DataType.F32_FracZ) \
    .get_op_info()

@op_info_register(apply_keras_momentum_op_info)
def _apply_keras_momentum_tbe():
    """ApplyKerasMomentum TBE register"""
    return
    