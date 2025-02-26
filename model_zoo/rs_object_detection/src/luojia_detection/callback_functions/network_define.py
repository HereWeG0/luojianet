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
"""training network wrapper."""

import time
import numpy as np
import luojianet_ms.nn as nn
from luojianet_ms.common.tensor import Tensor
from luojianet_ms.ops import composite as C
from luojianet_ms.ops import functional as F
from luojianet_ms import ParameterTuple
from luojianet_ms.train.callback import Callback
from luojianet_ms.nn.wrap.grad_reducer import DistributedGradReducer
from src.luojia_detection.configuration.config import config
import os

time_stamp_init = False
time_stamp_first = 0

class LossCallBack(Callback):
    """
    Monitor the loss in training.

    If the loss is NAN or INF terminating training.

    Note:
        If per_print_times is 0 do not print loss.

    Args:
        per_print_times (int): Print loss every times. Default: 1.
    """

    def __init__(self, per_print_times=1, rank_id=0):
        super(LossCallBack, self).__init__()
        if not isinstance(per_print_times, int) or per_print_times < 0:
            raise ValueError("print_step must be int and >= 0.")
        self._per_print_times = per_print_times
        self.count = 0
        self.loss_sum = 0
        self.rank_id = rank_id

        global time_stamp_init, time_stamp_first
        if not time_stamp_init:
            time_stamp_first = time.time()
            time_stamp_init = True

    def step_end(self, run_context):
        cb_params = run_context.original_args()
        loss = cb_params.net_outputs.asnumpy()
        cur_step_in_epoch = (cb_params.cur_step_num - 1) % cb_params.batch_num + 1

        self.count += 1
        self.loss_sum += float(loss)

        if self.count >= 1:
            global time_stamp_first
            time_stamp_current = time.time()
            total_loss = self.loss_sum / self.count
            save_dir = os.path.join(config.save_checkpoint_path, 'ckpt_' + str(self.rank_id) + '/')
            loss_file = open(save_dir + "loss.log", "a+")
            loss_file.write("%lu epoch: %s step: %s total_loss: %.5f" %
                            (time_stamp_current - time_stamp_first, cb_params.cur_epoch_num, cur_step_in_epoch,
                             total_loss))
            # print("%lu epoch: %s step: %s total_loss: %.5f" %
            #                 (time_stamp_current - time_stamp_first, cb_params.cur_epoch_num, cur_step_in_epoch,
            #                  total_loss))
            loss_file.write("\n")
            loss_file.close()

            self.count = 0
            self.loss_sum = 0

class LossNet(nn.Module):
    """loss method"""
    def call(self, x1, x2, x3, x4, x5, x6, x7=None):
        return x1 + x2

class WithLossCell(nn.Module):
    """
    Wrap the network with loss function to compute loss.

    Args:
        backbone (Cell): The target network to wrap.
        loss_fn (Cell): The loss function used to compute loss.
    """
    def __init__(self, backbone, loss_fn):
        super(WithLossCell, self).__init__(auto_prefix=False)
        self._backbone = backbone
        self._loss_fn = loss_fn
        self.mask_on = config.mask_on

    def call(self, x, img_shape, gt_bboxe, gt_label, gt_num, gt_mask=None):
        if self.mask_on:
            loss1, loss2, loss3, loss4, loss5, loss6, loss7 = self._backbone(x, img_shape, gt_bboxe, gt_label, gt_num,
                                                                             gt_mask)
            return self._loss_fn(loss1, loss2, loss3, loss4, loss5, loss6, loss7)
        else:
            loss1, loss2, loss3, loss4, loss5, loss6 = self._backbone(x, img_shape, gt_bboxe, gt_label, gt_num)
            return self._loss_fn(loss1, loss2, loss3, loss4, loss5, loss6)

    @property
    def backbone_network(self):
        """
        Get the backbone network.

        Returns:
            Cell, return backbone network.
        """
        return self._backbone


class TrainOneStepCell(nn.Module):
    """
    Network training package class.

    Append an optimizer to the training network after that the construct function
    can be called to create the backward graph.

    Args:
        network (Cell): The training network.
        optimizer (Cell): Optimizer for updating the weights.
        sens (Number): The adjust parameter. Default value is 1.0.
        reduce_flag (bool): The reduce flag. Default value is False.
        mean (bool): Allreduce method. Default value is False.
        degree (int): Device number. Default value is None.
    """
    def __init__(self, network, optimizer, sens=1.0, reduce_flag=False, mean=True, degree=None):
        super(TrainOneStepCell, self).__init__(auto_prefix=False)
        self.network = network
        self.network.set_grad()
        self.weights = ParameterTuple(network.trainable_params())
        self.optimizer = optimizer
        self.grad = C.GradOperation(get_by_list=True,
                                    sens_param=True)
        self.mask_on = config.mask_on

        if config.device_target == "Ascend":
            self.sens = Tensor((np.ones((1,)) * sens).astype(np.float16))
        else:
            self.sens = Tensor((np.ones((1,)) * sens).astype(np.float32))

        self.reduce_flag = reduce_flag
        self.hyper_map = C.HyperMap()
        if reduce_flag:
            self.grad_reducer = DistributedGradReducer(optimizer.parameters, mean, degree)

    def call(self, x, img_shape, gt_bboxe, gt_label, gt_num, gt_mask=None):
        weights = self.weights

        if self.mask_on:
            loss = self.network(x, img_shape, gt_bboxe, gt_label, gt_num, gt_mask)
            grads = self.grad(self.network, weights)(x, img_shape, gt_bboxe, gt_label, gt_num, gt_mask, self.sens)
        else:
            loss = self.network(x, img_shape, gt_bboxe, gt_label, gt_num)
            grads = self.grad(self.network, weights)(x, img_shape, gt_bboxe, gt_label, gt_num, self.sens)

        if self.reduce_flag:
            grads = self.grad_reducer(grads)
        return F.depend(loss, self.optimizer(grads))
