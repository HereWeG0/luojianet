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
"""Define framework enum data."""

from enum import Enum, IntEnum


class FileDataType(Enum):
    """Define framework file data type."""
    STEP_INFO = 'step_info'
    HASH_DIC = 'hash_dic'
    TENSOR_DATA_INFO = 'tensor_data_info'
    TASK_DESC_INFO = 'task_desc_info'

    @classmethod
    def members(cls):
        return {member.value for member in cls}


class VmDataType(IntEnum):
    """Definition of vm data type."""
    NUMBER_TYPE_BEGIN = 30
    BOOL = 31
    INT = 32
    INT8 = 33
    INT16 = 34
    INT32 = 35
    INT64 = 36
    UINT = 37
    UINT8 = 38
    UINT16 = 39
    UINT32 = 40
    UINT64 = 41
    FLOAT = 42
    FLOAT16 = 43
    FLOAT32 = 44
    FLOAT64 = 45
    COMPLEX = 46
    NUMBER_TYPE_END = 47

    @classmethod
    def get_data_type_name(cls, num):
        """
        Get the name of data type by enum number.

        Args:
            num (int): Enum number.

        Returns:
            str, the name of data type.
        """
        data_type = cls._value2member_map_.get(num)
        return 'UNKNOWN' if data_type is None else data_type.name


class VmFormat(IntEnum):
    """Define luojianet_ms data type."""
    UNKNOWN = 0
    DEFAULT = 1
    NC1KHKWHWC0 = 2
    ND = 3
    NCHW = 4
    NHWC = 5
    HWCN = 6
    NC1HWC0 = 7
    FRAC_Z = 8
    C1HWNCOC0 = 9
    FRAC_NZ = 10
    NC1HWC0_C04 = 11
    FRACTAL_Z_C04 = 12
    NDHWC = 13
    FRACTAL_ZN_LSTM = 14
    FRACTAL_ZN_RNN = 15
    ND_RNN_BIAS = 16
    NDC1HWC0 = 17
    NCDHW = 18
    FRACTAL_Z_3D = 19
    DHWNC = 20
    DHWCN = 21

    @classmethod
    def get_format_name(cls, num):
        """
        Get the name of format by enum number.

        Args:
            num (int): Enum number.

        Returns:
            str, the name of data type.
        """
        format_name = cls._value2member_map_.get(num)
        return 'UNKNOWN' if format_name is None else format_name.name
