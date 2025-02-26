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
"""test transform_to_bnn_layer"""

import numpy as np
from luojianet_ms import Tensor
from luojianet_ms.common.initializer import TruncatedNormal
import luojianet_ms.nn as nn
from luojianet_ms.nn import TrainOneStepCell, WithLossCell
from luojianet_ms.nn.probability import transforms, bnn_layers
import luojianet_ms.ops as ops
from luojianet_ms import context
from dataset import create_dataset


context.set_context(mode=context.GRAPH_MODE, device_target="GPU")


def conv(in_channels, out_channels, kernel_size, stride=1, padding=0):
    """weight initial for conv layer"""
    weight = weight_variable()
    return nn.Conv2d(in_channels, out_channels,
                     kernel_size=kernel_size, stride=stride, padding=padding,
                     weight_init=weight, has_bias=False, pad_mode="valid")


def fc_with_initialize(input_channels, out_channels):
    """weight initial for fc layer"""
    weight = weight_variable()
    bias = weight_variable()
    return nn.Dense(input_channels, out_channels, weight, bias)


def weight_variable():
    """weight initial"""
    return TruncatedNormal(0.02)


class LeNet5(nn.Module):
    """
    Lenet network

    Args:
        num_class (int): Num classes. Default: 10.

    Returns:
        Tensor, output tensor
    Examples:
        >>> LeNet5(num_class=10)

    """
    def __init__(self, num_class=10):
        super(LeNet5, self).__init__()
        self.num_class = num_class
        self.conv1 = conv(1, 6, 5)
        self.conv2 = conv(6, 16, 5)
        self.fc1 = fc_with_initialize(16 * 5 * 5, 120)
        self.fc2 = fc_with_initialize(120, 84)
        self.fc3 = fc_with_initialize(84, self.num_class)
        self.relu = nn.ReLU()
        self.max_pool2d = nn.MaxPool2d(kernel_size=2, stride=2)
        self.flatten = nn.Flatten()
        self.reshape = ops.Reshape()

    def call(self, x):
        x = self.conv1(x)
        x = self.relu(x)
        x = self.max_pool2d(x)
        x = self.conv2(x)
        x = self.relu(x)
        x = self.max_pool2d(x)
        x = self.flatten(x)
        x = self.fc1(x)
        x = self.relu(x)
        x = self.fc2(x)
        x = self.relu(x)
        x = self.fc3(x)
        return x


def train_model(train_net, net, dataset):
    accs = []
    loss_sum = 0
    for _, data in enumerate(dataset.create_dict_iterator(output_numpy=True, num_epochs=1)):
        train_x = Tensor(data['image'].astype(np.float32))
        label = Tensor(data['label'].astype(np.int32))
        loss = train_net(train_x, label)
        output = net(train_x)
        log_output = ops.LogSoftmax(axis=1)(output)
        acc = np.mean(log_output.asnumpy().argmax(axis=1) == label.asnumpy())
        accs.append(acc)
        loss_sum += loss.asnumpy()

    loss_sum = loss_sum / len(accs)
    acc_mean = np.mean(accs)
    return loss_sum, acc_mean


def validate_model(net, dataset):
    accs = []
    for _, data in enumerate(dataset.create_dict_iterator(output_numpy=True, num_epochs=1)):
        train_x = Tensor(data['image'].astype(np.float32))
        label = Tensor(data['label'].astype(np.int32))
        output = net(train_x)
        log_output = ops.LogSoftmax(axis=1)(output)
        acc = np.mean(log_output.asnumpy().argmax(axis=1) == label.asnumpy())
        accs.append(acc)

    acc_mean = np.mean(accs)
    return acc_mean


if __name__ == "__main__":
    network = LeNet5()

    criterion = nn.SoftmaxCrossEntropyWithLogits(sparse=True, reduction="mean")
    optimizer = nn.AdamWeightDecay(params=network.trainable_params(), learning_rate=0.0001)

    net_with_loss = WithLossCell(network, criterion)
    train_network = TrainOneStepCell(net_with_loss, optimizer)

    bnn_transformer = transforms.TransformToBNN(train_network, 60000, 0.000001)
    train_bnn_network = bnn_transformer.transform_to_bnn_layer(nn.Conv2d, bnn_layers.ConvReparam)
    # train_bnn_network = bnn_transformer.transform_to_bnn_layer(nn.Dense, bnn_layers.DenseReparam)
    train_bnn_network.set_train()

    train_set = create_dataset('/home/workspace/luojianet_ms_dataset/mnist_data/train', 64, 1)
    test_set = create_dataset('/home/workspace/luojianet_ms_dataset/mnist_data/test', 64, 1)

    epoch = 100

    for i in range(epoch):
        train_loss, train_acc = train_model(train_bnn_network, network, train_set)

        valid_acc = validate_model(network, test_set)

        print('Epoch: {} \tTraining Loss: {:.4f} \tTraining Accuracy: {:.4f} \tvalidation Accuracy: {:.4f}'.format(
            i, train_loss, train_acc, valid_acc))
