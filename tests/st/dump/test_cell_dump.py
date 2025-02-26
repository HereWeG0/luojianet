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
import os
import sys
import tempfile
import time
import shutil
import glob

from enum import Enum
import numpy as np
import pytest
from luojianet_ms import Tensor, set_dump
from luojianet_ms.ops import operations as P
from luojianet_ms.nn import Module
from luojianet_ms.nn import Dense
from luojianet_ms.nn import SoftmaxCrossEntropyWithLogits
from luojianet_ms.nn import Momentum
from luojianet_ms.nn import TrainOneStepCell
from luojianet_ms.nn import WithLossCell
from dump_test_utils import generate_cell_dump_json, check_dump_structure
from tests.security_utils import security_off_wrap

class IsDump(Enum):
    SET_DUMP_TRUE = 1
    SET_DUMP_FALSE = 2
    SET_NONE = 3

class ReluReduceMeanDenseRelu(Module):
    def __init__(self, kernel, bias, in_channel, num_class):
        super().__init__()
        self.relu = P.ReLU()
        self.mean = P.ReduceMean(keep_dims=False)
        self.dense = Dense(in_channel, num_class, kernel, bias)

    def call(self, x_):
        x_ = self.relu(x_)
        x_ = self.mean(x_, (2, 3))
        x_ = self.dense(x_)
        x_ = self.relu(x_)
        return x_


def run_multi_layer_train(is_set_dump):
    weight = Tensor(np.ones((1000, 2048)).astype(np.float32))
    bias = Tensor(np.ones((1000,)).astype(np.float32))
    net = ReluReduceMeanDenseRelu(weight, bias, 2048, 1000)
    if is_set_dump is IsDump.SET_DUMP_TRUE:
        set_dump(net.relu)
    elif is_set_dump is IsDump.SET_DUMP_FALSE:
        set_dump(net.relu, enabled=False)
        set_dump(net.mean)
    criterion = SoftmaxCrossEntropyWithLogits(sparse=False)
    optimizer = Momentum(learning_rate=0.1, momentum=0.1,
                         params=filter(lambda x: x.requires_grad, net.get_parameters()))
    net_with_criterion = WithLossCell(net, criterion)
    train_network = TrainOneStepCell(net_with_criterion, optimizer)
    train_network.set_train()
    inputs = Tensor(np.random.randn(32, 2048, 7, 7).astype(np.float32))
    label = Tensor(np.zeros(shape=(32, 1000)).astype(np.float32))
    train_network(inputs, label)


@pytest.mark.level0
@pytest.mark.platform_arm_ascend_training
@pytest.mark.platform_x86_ascend_training
@pytest.mark.env_onecard
@security_off_wrap
def test_ascend_cell_dump():
    """
    Feature: Module Dump
    Description: Test cell dump
    Expectation: Only dump cell set by set_dump when dump_mode = 2
    """
    if sys.platform != 'linux':
        return
    with tempfile.TemporaryDirectory(dir='/tmp') as tmp_dir:
        dump_path = os.path.join(tmp_dir, 'cell_dump')
        dump_config_path = os.path.join(tmp_dir, 'cell_dump.json')
        generate_cell_dump_json(dump_path, dump_config_path, 'test_async_dump', 2)
        os.environ['LUOJIANET_DUMP_CONFIG'] = dump_config_path
        if os.path.isdir(dump_path):
            shutil.rmtree(dump_path)
        run_multi_layer_train(IsDump.SET_DUMP_TRUE)
        dump_file_path = os.path.join(dump_path, 'rank_0', 'Net', '0', '0')
        for _ in range(5):
            if not os.path.exists(dump_file_path):
                time.sleep(2)
        check_dump_structure(dump_path, dump_config_path, 1, 1, 1)

        # make sure 2 relu dump files are generated with correct name prefix
        assert len(os.listdir(dump_file_path)) == 2
        relu_file_name = "ReLU.Default_network-WithLossCell__backbone-ReluReduceMeanDenseRelu_ReLU-op*.*.*.*"
        relu_file1 = glob.glob(os.path.join(dump_file_path, relu_file_name))[0]
        relu_file2 = glob.glob(os.path.join(dump_file_path, relu_file_name))[1]
        assert relu_file1
        assert relu_file2
        del os.environ['LUOJIANET_DUMP_CONFIG']


@pytest.mark.level0
@pytest.mark.platform_arm_ascend_training
@pytest.mark.platform_x86_ascend_training
@pytest.mark.env_onecard
@security_off_wrap
def test_ascend_not_cell_dump():
    """
    Feature: Module Dump
    Description: Test cell dump
    Expectation: Should ignore set_dump when dump_mode != 2
    """
    if sys.platform != 'linux':
        return
    with tempfile.TemporaryDirectory(dir='/tmp') as tmp_dir:
        dump_path = os.path.join(tmp_dir, 'cell_dump')
        dump_config_path = os.path.join(tmp_dir, 'cell_dump.json')
        generate_cell_dump_json(dump_path, dump_config_path, 'test_async_dump', 0)
        os.environ['LUOJIANET_DUMP_CONFIG'] = dump_config_path
        if os.path.isdir(dump_path):
            shutil.rmtree(dump_path)
        run_multi_layer_train(IsDump.SET_DUMP_TRUE)
        dump_file_path = os.path.join(dump_path, 'rank_0', 'Net', '0', '0')
        for _ in range(5):
            if not os.path.exists(dump_file_path):
                time.sleep(2)
        check_dump_structure(dump_path, dump_config_path, 1, 1, 1)

        # make sure set_dump is ignored and all cell layer are dumped
        assert len(os.listdir(dump_file_path)) == 10
        del os.environ['LUOJIANET_DUMP_CONFIG']


@pytest.mark.level0
@pytest.mark.platform_arm_ascend_training
@pytest.mark.platform_x86_ascend_training
@pytest.mark.env_onecard
@security_off_wrap
def test_ascend_cell_empty_dump():
    """
    Feature: Module Dump
    Description: Test cell dump
    Expectation: Should dump nothing when set_dump is not set and dump_mode = 2
    """
    if sys.platform != 'linux':
        return
    with tempfile.TemporaryDirectory(dir='/tmp') as tmp_dir:
        dump_path = os.path.join(tmp_dir, 'cell_dump')
        dump_config_path = os.path.join(tmp_dir, 'cell_dump.json')
        generate_cell_dump_json(dump_path, dump_config_path, 'test_async_dump', 2)
        os.environ['LUOJIANET_DUMP_CONFIG'] = dump_config_path
        if os.path.isdir(dump_path):
            shutil.rmtree(dump_path)
        run_multi_layer_train(IsDump.SET_NONE)
        dump_file_path = os.path.join(dump_path, 'rank_0', 'Net')
        time.sleep(5)

        # make sure no files are dumped
        assert not os.path.exists(dump_file_path)
        del os.environ['LUOJIANET_DUMP_CONFIG']

@pytest.mark.level0
@pytest.mark.platform_arm_ascend_training
@pytest.mark.platform_x86_ascend_training
@pytest.mark.env_onecard
@security_off_wrap
def test_ascend_cell_dump_set_enable_false():
    """
    Feature: Module Dump
    Description: Test cell dump
    Expectation: Should ignore set_dump when enabled=False
    """
    if sys.platform != 'linux':
        return
    with tempfile.TemporaryDirectory(dir='/tmp') as tmp_dir:
        dump_path = os.path.join(tmp_dir, 'cell_dump')
        dump_config_path = os.path.join(tmp_dir, 'cell_dump.json')
        generate_cell_dump_json(dump_path, dump_config_path, 'test_async_dump', 2)
        os.environ['LUOJIANET_DUMP_CONFIG'] = dump_config_path
        if os.path.isdir(dump_path):
            shutil.rmtree(dump_path)
        run_multi_layer_train(IsDump.SET_DUMP_FALSE)
        dump_file_path = os.path.join(dump_path, 'rank_0', 'Net', '0', '0')
        for _ in range(5):
            if not os.path.exists(dump_file_path):
                time.sleep(1)
        check_dump_structure(dump_path, dump_config_path, 1, 1, 1)

        # make sure directory has dumped files with enabled=True
        assert len(os.listdir(dump_file_path)) == 1
        mean_file_name = "ReduceMean.Default_network-WithLossCell__backbone-ReluReduceMeanDenseRelu_ReduceMean-*.*.*.*"
        mean_file = glob.glob(os.path.join(dump_file_path, mean_file_name))[0]
        assert mean_file
        del os.environ['LUOJIANET_DUMP_CONFIG']
