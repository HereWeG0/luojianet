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

"""ResizeNearestNeighborgrad op"""
from luojianet_ms.ops.op_info_register import op_info_register, TBERegOp, DataType

resize_nearest_neighbor_grad_op_info = TBERegOp("ResizeNearestNeighborGrad") \
    .fusion_type("OPAQUE") \
    .async_flag(False) \
    .binfile_name("resize_nearest_neighbor_grad.so") \
    .compute_cost(10) \
    .kernel_name("resize_nearest_neighbor_v2_grad") \
    .partial_flag(True) \
    .dynamic_shape(True) \
    .need_check_supported(True) \
    .attr("size", "required", "listInt", "all") \
    .attr("align_corners", "optional", "bool", "all", "false") \
    .attr("half_pixel_centers", "optional", "bool", "all", "false") \
    .input(0, "grads", False, "required", "all") \
    .input(1, "size", False, "required", "all") \
    .output(0, "y", False, "required", "all") \
    .dtype_format(DataType.F32_5HD, DataType.I32_5HD, DataType.F32_5HD) \
    .get_op_info()


@op_info_register(resize_nearest_neighbor_grad_op_info)
def _resize_nearest_neighbor_grad_ds_tbe():
    """ResizeNearestNeighborGrad TBE register"""
    return
