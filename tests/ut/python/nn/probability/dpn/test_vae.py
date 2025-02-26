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
""" test VAE interface """
import numpy as np

import luojianet_ms.common.dtype as mstype
import luojianet_ms.nn as nn
from luojianet_ms import Tensor
from luojianet_ms.common.api import _cell_graph_executor
from luojianet_ms.nn.probability.dpn import VAE


class Encoder(nn.Module):
    def __init__(self):
        super(Encoder, self).__init__()
        self.fc1 = nn.Dense(6, 3)
        self.relu = nn.ReLU()

    def call(self, x):
        x = self.fc1(x)
        x = self.relu(x)
        return x


class Decoder(nn.Module):
    def __init__(self):
        super(Decoder, self).__init__()
        self.fc1 = nn.Dense(3, 6)
        self.sigmoid = nn.Sigmoid()

    def call(self, z):
        z = self.fc1(z)
        z = self.sigmoid(z)
        return z


def test_vae():
    """
    Test the vae interface with the DNN model.
    """
    encoder = Encoder()
    decoder = Decoder()
    net = VAE(encoder, decoder, hidden_size=3, latent_size=2)
    input_data = Tensor(np.random.rand(32, 6), dtype=mstype.float32)
    _cell_graph_executor.compile(net, input_data)
