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
test psnr
"""
import numpy as np
import pytest

import luojianet_ms.nn as nn
from luojianet_ms import Tensor
from luojianet_ms.common import dtype as mstype
from luojianet_ms.common.api import _cell_graph_executor


class PSNRNet(nn.Module):
    def __init__(self, max_val=1.0):
        super(PSNRNet, self).__init__()
        self.net = nn.PSNR(max_val)

    def call(self, img1, img2):
        return self.net(img1, img2)


def test_compile_psnr():
    max_val = 1.0
    net = PSNRNet(max_val)
    img1 = Tensor(np.random.random((8, 3, 16, 16)))
    img2 = Tensor(np.random.random((8, 3, 16, 16)))
    _cell_graph_executor.compile(net, img1, img2)


def test_compile_psnr_grayscale():
    max_val = 255
    net = PSNRNet(max_val)
    img1 = Tensor(np.random.randint(0, 256, (8, 1, 16, 16), np.uint8))
    img2 = Tensor(np.random.randint(0, 256, (8, 1, 16, 16), np.uint8))
    _cell_graph_executor.compile(net, img1, img2)


def test_psnr_max_val_negative():
    max_val = -1
    with pytest.raises(ValueError):
        _ = PSNRNet(max_val)


def test_psnr_max_val_bool():
    max_val = True
    with pytest.raises(TypeError):
        _ = PSNRNet(max_val)


def test_psnr_max_val_zero():
    max_val = 0
    with pytest.raises(ValueError):
        _ = PSNRNet(max_val)


def test_psnr_different_shape():
    shape_1 = (8, 3, 16, 16)
    shape_2 = (8, 3, 8, 8)
    img1 = Tensor(np.random.random(shape_1))
    img2 = Tensor(np.random.random(shape_2))
    net = PSNRNet()
    with pytest.raises(ValueError):
        _cell_graph_executor.compile(net, img1, img2)


def test_psnr_different_dtype():
    dtype_1 = mstype.float32
    dtype_2 = mstype.float16
    img1 = Tensor(np.random.random((8, 3, 16, 16)), dtype=dtype_1)
    img2 = Tensor(np.random.random((8, 3, 16, 16)), dtype=dtype_2)
    net = PSNRNet()
    with pytest.raises(TypeError):
        _cell_graph_executor.compile(net, img1, img2)


def test_psnr_invalid_5d_input():
    shape_1 = (8, 3, 16, 16)
    shape_2 = (8, 3, 8, 8)
    invalid_shape = (8, 3, 16, 16, 1)
    img1 = Tensor(np.random.random(shape_1))
    invalid_img1 = Tensor(np.random.random(invalid_shape))
    img2 = Tensor(np.random.random(shape_2))
    invalid_img2 = Tensor(np.random.random(invalid_shape))

    net = PSNRNet()
    with pytest.raises(ValueError):
        _cell_graph_executor.compile(net, invalid_img1, img2)
    with pytest.raises(ValueError):
        _cell_graph_executor.compile(net, img1, invalid_img2)
    with pytest.raises(ValueError):
        _cell_graph_executor.compile(net, invalid_img1, invalid_img2)
