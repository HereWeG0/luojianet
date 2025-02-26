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
import random
import argparse
import numpy as np
from resnet import resnet50

import luojianet_ms.common.dtype as mstype
import luojianet_ms.ops.functional as F
from luojianet_ms.train.callback import ModelCheckpoint, CheckpointConfig, LossMonitor, TimeMonitor
from luojianet_ms.train.serialization import load_checkpoint, load_param_into_net
import luojianet_ms.dataset as ds
import luojianet_ms.dataset.transforms.c_transforms as C
import luojianet_ms.dataset.vision.c_transforms as vision
import luojianet_ms.nn as nn
from luojianet_ms import Tensor
from luojianet_ms import context
from luojianet_ms.communication.management import init
from luojianet_ms.nn.optim.momentum import Momentum
from luojianet_ms.ops import operations as P
from luojianet_ms.train.model import Model
from luojianet_ms.context import ParallelMode

random.seed(1)
np.random.seed(1)
ds.config.set_seed(1)

parser = argparse.ArgumentParser(description='Image classification')
parser.add_argument('--run_distribute', type=bool, default=False, help='Run distribute')
parser.add_argument('--device_num', type=int, default=1, help='Device num.')
parser.add_argument('--do_train', type=bool, default=True, help='Do train or not.')
parser.add_argument('--do_eval', type=bool, default=False, help='Do eval or not.')
parser.add_argument('--epoch_size', type=int, default=1, help='Epoch size.')
parser.add_argument('--batch_size', type=int, default=32, help='Batch size.')
parser.add_argument('--num_classes', type=int, default=10, help='Num classes.')
parser.add_argument('--checkpoint_path', type=str, default=None, help='Checkpoint file path')
parser.add_argument('--dataset_path', type=str, default="/var/log/npu/datasets/cifar", help='Dataset path')
args_opt = parser.parse_args()

device_id = int(os.getenv('DEVICE_ID'))

data_home = args_opt.dataset_path

context.set_context(mode=context.GRAPH_MODE, device_target="Ascend")
context.set_context(device_id=device_id)


def create_dataset(repeat_num=1, training=True):
    data_dir = data_home + "/cifar-10-batches-bin"
    if not training:
        data_dir = data_home + "/cifar-10-verify-bin"
    data_set = ds.Cifar10Dataset(data_dir, num_samples=32)

    if args_opt.run_distribute:
        rank_id = int(os.getenv('RANK_ID'))
        rank_size = int(os.getenv('RANK_SIZE'))
        data_set = ds.Cifar10Dataset(data_dir, num_shards=rank_size, shard_id=rank_id, num_samples=32)

    resize_height = 224
    resize_width = 224
    rescale = 1.0 / 255.0
    shift = 0.0

    # define map operations
    random_crop_op = vision.RandomCrop((32, 32), (4, 4, 4, 4))  # padding_mode default CONSTANT
    random_horizontal_op = vision.RandomHorizontalFlip()
    resize_op = vision.Resize((resize_height, resize_width))  # interpolation default BILINEAR
    rescale_op = vision.Rescale(rescale, shift)
    normalize_op = vision.Normalize((0.4465, 0.4822, 0.4914), (0.2010, 0.1994, 0.2023))
    changeswap_op = vision.HWC2CHW()
    type_cast_op = C.TypeCast(mstype.int32)

    c_trans = []
    if training:
        c_trans = [random_crop_op, random_horizontal_op]
    c_trans += [resize_op, rescale_op, normalize_op,
                changeswap_op]

    # apply map operations on images
    data_set = data_set.map(operations=type_cast_op, input_columns="label")
    data_set = data_set.map(operations=c_trans, input_columns="image")

    # apply repeat operations
    data_set = data_set.repeat(repeat_num)

    # apply shuffle operations
    data_set = data_set.shuffle(buffer_size=10)

    # apply batch operations
    data_set = data_set.batch(batch_size=args_opt.batch_size, drop_remainder=True)

    return data_set


class CrossEntropyLoss(nn.Module):
    def __init__(self):
        super(CrossEntropyLoss, self).__init__()
        self.cross_entropy = P.SoftmaxCrossEntropyWithLogits()
        self.mean = P.ReduceMean()
        self.one_hot = P.OneHot()
        self.one = Tensor(1.0, mstype.float32)
        self.zero = Tensor(0.0, mstype.float32)

    def call(self, logits, label):
        label = self.one_hot(label, F.shape(logits)[1], self.one, self.zero)
        loss_func = self.cross_entropy(logits, label)[0]
        loss_func = self.mean(loss_func, (-1,))
        return loss_func


if __name__ == '__main__':
    if not args_opt.do_eval and args_opt.run_distribute:
        context.set_auto_parallel_context(device_num=args_opt.device_num, parallel_mode=ParallelMode.DATA_PARALLEL,
                                          all_reduce_fusion_config=[140])
        init()

    context.set_context(mode=context.GRAPH_MODE)
    epoch_size = args_opt.epoch_size
    net = resnet50(args_opt.batch_size, args_opt.num_classes)
    loss = CrossEntropyLoss()
    opt = Momentum(filter(lambda x: x.requires_grad, net.get_parameters()), 0.01, 0.9)

    model = Model(net, loss_fn=loss, optimizer=opt, metrics={'acc'})

    if args_opt.do_train:
        dataset = create_dataset(1)
        batch_num = dataset.get_dataset_size()
        config_ck = CheckpointConfig(save_checkpoint_steps=batch_num * 5, keep_checkpoint_max=10)
        ckpoint_cb = ModelCheckpoint(prefix="train_resnet_cifar10", directory="./", config=config_ck)
        time_cb = TimeMonitor(data_size=batch_num)
        loss_cb = LossMonitor()
        model.train(epoch_size, dataset, callbacks=[ckpoint_cb, loss_cb, time_cb])

    if args_opt.do_eval:
        if args_opt.checkpoint_path:
            param_dict = load_checkpoint(args_opt.checkpoint_path)
            load_param_into_net(net, param_dict)
        net.set_train(False)
        eval_dataset = create_dataset(1, training=False)
        res = model.eval(eval_dataset)
        print("result: ", res)
