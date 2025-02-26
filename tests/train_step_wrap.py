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
train step wrap
"""
import luojianet_ms.nn as nn
from luojianet_ms import ParameterTuple
from luojianet_ms.ops import composite as C


class TrainStepWrap(nn.Module):
    """
    TrainStepWrap definition
    """

    def __init__(self, network):
        super(TrainStepWrap, self).__init__()
        self.network = network
        self.network.set_train()
        self.weights = ParameterTuple(network.trainable_params())
        self.optimizer = nn.Momentum(self.weights, 0.1, 0.9)
        self.hyper_map = C.HyperMap()
        self.grad = C.GradOperation(get_by_list=True)

    def call(self, x, label):
        weights = self.weights
        grads = self.grad(self.network, weights)(x, label)
        return self.optimizer(grads)


class NetWithLossClass(nn.Module):
    """
    NetWithLossClass definition
    """

    def __init__(self, network):
        super(NetWithLossClass, self).__init__(auto_prefix=False)
        self.loss = nn.SoftmaxCrossEntropyWithLogits()
        self.network = network

    def call(self, x, label):
        predict = self.network(x)
        return self.loss(predict, label)


def train_step_with_loss_warp(network):
    return TrainStepWrap(NetWithLossClass(network))


class TrainStepWrap2(nn.Module):
    """
    TrainStepWrap2 definition
    """

    def __init__(self, network, sens):
        super(TrainStepWrap2, self).__init__()
        self.network = network
        self.network.set_train()
        self.weights = ParameterTuple(network.get_parameters())
        self.optimizer = nn.Momentum(self.weights, 0.1, 0.9)
        self.hyper_map = C.HyperMap()
        self.grad = C.GradOperation(get_by_list=True, sens_param=True)
        self.sens = sens

    def call(self, x):
        weights = self.weights
        grads = self.grad(self.network, weights)(x, self.sens)
        return self.optimizer(grads)


def train_step_with_sens(network, sens):
    return TrainStepWrap2(network, sens)


class TrainStepWrapWithoutOpt(nn.Module):
    """
    TrainStepWrapWithoutOpt definition
    """

    def __init__(self, network):
        super(TrainStepWrapWithoutOpt, self).__init__()
        self.network = network
        self.weights = ParameterTuple(network.trainable_params())
        self.grad = C.GradOperation(get_by_list=True)

    def call(self, x, label):
        grads = self.grad(self.network, self.weights)(x, label)
        return grads


def train_step_without_opt(network):
    return TrainStepWrapWithoutOpt(NetWithLossClass(network))
