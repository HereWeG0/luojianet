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
"""optimizer"""
import inspect
from typing import Iterable

import numpy as np

import luojianet_ms
from luojianet_ms.ops import functional as F, composite as C, operations as P
from luojianet_ms.ops.operations import _inner_ops as inner
from luojianet_ms.nn.cell import Module
from luojianet_ms.nn.layer.container import CellList
from luojianet_ms.common.parameter import Parameter, ParameterTuple
from luojianet_ms.common.initializer import initializer
from luojianet_ms.common.tensor import Tensor, RowTensor
import luojianet_ms.common.dtype as mstype
from luojianet_ms._checkparam import Validator as validator
from luojianet_ms import log as logger
from luojianet_ms.parallel._utils import _get_global_rank, _get_device_num, _get_parallel_mode
from luojianet_ms.context import ParallelMode
from luojianet_ms import context
from luojianet_ms.nn.learning_rate_schedule import LearningRateSchedule

__all__ = ['Optimizer', 'opt_init_args_register']


def opt_init_args_register(fn):
    """Register optimizer init args."""
    def deco(self, *args, **kwargs):
        bound_args = inspect.signature(fn).bind(self, *args, **kwargs)
        bound_args.apply_defaults()
        arguments = bound_args.arguments
        arguments.pop('self')
        if 'params' in arguments.keys():
            setattr(self, 'init_params', dict({"params": arguments['params']}))
            arguments.pop('params')
        if 'optimizer' in arguments.keys():
            setattr(self, 'init_params', dict({"params": arguments['optimizer'].init_params["params"]}))
            arguments.pop('optimizer')
        setattr(self, 'init_args', arguments)
        fn(self, *args, **kwargs)
    return deco


class Optimizer(Module):
    """
    Base class for updating parameters. Never use this class directly, but instantiate one of its subclasses instead.

    Grouping parameters is supported. If parameters are grouped, different strategy of `learning_rate`, `weight_decay`
    and `grad_centralization` can be applied to each group.

    Note:
        If parameters are not grouped, the `weight_decay` in optimizer will be applied on the network parameters without
        'beta' or 'gamma' in their names. Users can group parameters to change the strategy of decaying weight. When
        parameters are grouped, each group can set `weight_decay`, if not, the `weight_decay` in optimizer will be
        applied.

    Args:
        learning_rate (Union[float, int, Tensor, Iterable, LearningRateSchedule]):

            - float: The fixed learning rate value. Must be equal to or greater than 0.

            - int: The fixed learning rate value. Must be equal to or greater than 0. It will be converted to float.

            - Tensor: Its value should be a scalar or a 1-D vector. For scalar, fixed learning rate will be applied.
              For vector, learning rate is dynamic, then the i-th step will take the i-th value as the learning rate.

            - Iterable: Learning rate is dynamic. The i-th step will take the i-th value as the learning rate.

            - LearningRateSchedule: Learning rate is dynamic. During training, the optimizer calls the instance of
              LearningRateSchedule with step as the input to get the learning rate of current step.

        parameters (Union[list[Parameter], list[dict]]): Must be list of `Parameter` or list of `dict`. When the
            `parameters` is a list of `dict`, the string "params", "lr", "weight_decay", "grad_centralization" and
            "order_params" are the keys can be parsed.

            - params: Required. Parameters in current group. The value must be a list of `Parameter`.

            - lr: Optional. If "lr" in the keys, the value of corresponding learning rate will be used.
              If not, the `learning_rate` in optimizer will be used. Fixed and dynamic learning rate are supported.

            - weight_decay: Optional. If "weight_decay" in the keys, the value of corresponding weight decay
              will be used. If not, the `weight_decay` in the optimizer will be used.

            - grad_centralization: Optional. Must be Boolean. If "grad_centralization" is in the keys, the set value
              will be used. If not, the `grad_centralization` is False by default. This configuration only works on the
              convolution layer.

            - order_params: Optional. When parameters is grouped, this usually is used to maintain the order of
              parameters that appeared in the network to improve performance. The value should be parameters whose
              order will be followed in optimizer.
              If `order_params` in the keys, other keys will be ignored and the element of 'order_params' must be in
              one group of `params`.

        weight_decay (Union[float, int]): An int or a floating point value for the weight decay.
            It must be equal to or greater than 0.
            If the type of `weight_decay` input is int, it will be converted to float. Default: 0.0.
        loss_scale (float): A floating point value for the loss scale. It must be greater than 0. If the
            type of `loss_scale` input is int, it will be converted to float. In general, use the default value. Only
            when `FixedLossScaleManager` is used for training and the `drop_overflow_update` in
            `FixedLossScaleManager` is set to False, this value needs to be the same as the `loss_scale` in
            `FixedLossScaleManager`. Refer to class :class:`luojianet_ms.FixedLossScaleManager` for more details.
            Default: 1.0.

    Raises:
        TypeError: If `learning_rate` is not one of int, float, Tensor, Iterable, LearningRateSchedule.
        TypeError: If element of `parameters` is neither Parameter nor dict.
        TypeError: If `loss_scale` is not a float.
        TypeError: If `weight_decay` is neither float nor int.
        ValueError: If `loss_scale` is less than or equal to 0.
        ValueError: If `weight_decay` is less than 0.
        ValueError: If `learning_rate` is a Tensor, but the dimension of tensor is greater than 1.

    Supported Platforms:
        ``Ascend`` ``GPU`` ``CPU``
    """

    def __init__(self, learning_rate, parameters, weight_decay=0.0, loss_scale=1.0):
        super(Optimizer, self).__init__(auto_prefix=False)
        parameters = self._parameters_base_check(parameters, "parameters")
        if not all(isinstance(x, Parameter) for x in parameters) and not all(isinstance(x, dict) for x in parameters):
            raise TypeError("For 'Optimizer', all elements of the argument 'parameters' must be "
                            "'Parameter' or 'dict'.")

        if isinstance(loss_scale, int):
            loss_scale = float(loss_scale)
        validator.check_value_type("loss_scale", loss_scale, [float], self.cls_name)
        validator.check_positive_float(loss_scale, "loss_scale", self.cls_name)
        self.loss_scale = loss_scale

        weight_decay = self._preprocess_weight_decay(weight_decay)
        self.grad_centralization = False

        self._unique = True
        self._target = context.get_context("device_target")
        self.dynamic_lr = False
        self.assignadd = None
        self.global_step = None
        self.is_group = False
        self.is_group_lr = False
        self.is_group_params_ordered = False
        learning_rate = self._preprocess_single_lr(learning_rate)
        if isinstance(parameters[0], dict):
            self.is_group = True
            self.group_params = []
            self.group_lr = []
            self.group_weight_decay = []
            self.group_grad_centralization = []
            self._init_group_params(parameters, learning_rate, weight_decay, self.grad_centralization)

        # The final value of dynamic_lr can be determined after the process of parse_single_lr and init_group_params
        if self.dynamic_lr:
            self.assignadd = P.AssignAdd()
            self.global_step = Parameter(initializer(0, [1], luojianet_ms.int32), name='global_step')

        if self.is_group_lr:
            self.learning_rate = CellList(self.group_lr, auto_prefix=False) if self.dynamic_lr \
                else ParameterTuple(self.group_lr)
        else:
            self.learning_rate = self._build_single_lr(learning_rate, 'learning_rate')

        if self.is_group:
            self.parameters = ParameterTuple(self.group_params)
            self.weight_decay = tuple(self.group_weight_decay)
            self.weight_decay_tensor_tuple = tuple(Tensor(x, mstype.float32) for x in self.group_weight_decay)
            decay_filter = lambda x: x > 0
            self.decay_flags = tuple(decay_filter(x) for x in self.weight_decay)
            self.exec_weight_decay = any(self.decay_flags)
            self.grad_centralization_flags = tuple(self.group_grad_centralization)
        else:
            self.parameters = ParameterTuple(parameters)
            self.weight_decay = weight_decay * loss_scale
            self.weight_decay_tensor = Tensor(self.weight_decay, mstype.float32)
            decay_filter = lambda x: 'beta' not in x.name and 'gamma' not in x.name
            self.decay_flags = tuple(decay_filter(x) for x in self.parameters)
            self.exec_weight_decay = self.weight_decay > 0
        # when a parameter has been unique, there is no need do another unique in optimizer.
        for param in self.parameters:
            if param.unique:
                self._unique = False
                break
        ps_filter = lambda x: x.is_param_ps
        self.ps_parameters = tuple(ps_filter(x) for x in self.parameters)
        cache_filter = lambda x: x.cache_enable
        self.cache_enable = tuple(cache_filter(x) for x in self.parameters)
        self.reciprocal_scale = Tensor(1.0 / loss_scale, mstype.float32)
        self.need_scale = loss_scale != 1.0
        self.global_step_increase_tensor = Tensor(1, mstype.int32)
        self.param_length = len(self.parameters)
        self.map_ = C.Map()
        self.map_reverse = C.Map(None, True)
        self.hyper_map = C.HyperMap()
        self.hyper_map_reverse = C.HyperMap(None, True)
        self._use_parallel_optimizer()
        self.enable_tuple_broaden = True

    def _use_parallel_optimizer(self):
        """Indicates whether to use automatic parallelism."""
        if context.get_auto_parallel_context("enable_parallel_optimizer"):
            if _get_parallel_mode() == ParallelMode.DATA_PARALLEL and context.get_context("device_target") == "Ascend":
                self.use_parallel = True
            elif _get_parallel_mode() == ParallelMode.DATA_PARALLEL \
                    and context.get_context("device_target") != "Ascend":
                raise RuntimeError("Parallel optimizer only supports 'Ascend' in data parallel mode, "
                                   "but got {}.".format(context.get_context("device_target")))
            elif _get_parallel_mode() in (ParallelMode.STAND_ALONE, ParallelMode.HYBRID_PARALLEL):
                raise RuntimeError("Parallel optimizer is not supported in {}.".format(_get_parallel_mode()))
            else:
                self.use_parallel = False
        else:
            self.use_parallel = False
        if self.use_parallel:
            if self.cls_name not in ["Lamb", "AdamWeightDecay", "AdaFactor"]:
                raise RuntimeError("Parallel optimizer only support optimizer 'Lamb', 'AdamWeightDecay' or "
                                   "'AdaFactor', but got {}.".format(self.cls_name))
            self.dev_num = _get_device_num()
            if self.dev_num > self.param_length:
                raise RuntimeError("Parallel optimizer can not be applied when the number of parameters {} is"
                                   " less than the number of devices {}".format(self.param_length, self.dev_num))
            self.param_rank = self._get_parameter_group_id()
            self.optim_filter = tuple(map(lambda x: x == _get_global_rank(), self.param_rank))
            self.param_names = []
            for param in self.parameters:
                self.param_names.append(param.name)
        else:
            self.optim_filter = (True,) * self.param_length

    @property
    def unique(self):
        """
        Whether to make the gradients unique in optimizer. Generally, it is used in sparse networks. Set to True if the
        gradients of the optimizer are sparse. Set to False if the forward network has made the parameters unique,
        that is, the gradients of the optimizer is no longer sparse.
        The default value is True when it is not set.
        """
        return self._unique

    @unique.setter
    def unique(self, value):
        """Set the `unique` attribute."""
        if not isinstance(value, bool):
            raise TypeError("For 'Optimizer', the property 'unique' must be bool, "
                            "but got {}".format(type(value)))
        self._unique = value

    @property
    def target(self):
        """
        The property is used to determine whether the parameter is updated on host or device. The input type is str
        and can only be 'CPU', 'Ascend' or 'GPU'.
        """
        return self._target

    @target.setter
    def target(self, value):
        """
        If the input value is set to "CPU", the parameters will be updated on the host using the Fused
        optimizer operation.
        """
        raise NotImplementedError

    def _set_base_target(self, value):
        """
        If the input value is set to "CPU", the parameters will be updated on the host using the Fused
        optimizer operation.
        """
        if not isinstance(value, str):
            raise TypeError("For 'Optimizer', the property 'target' must be string, but got {}".format(type(value)))

        if value not in ('CPU', 'Ascend', 'GPU'):
            raise ValueError("For 'Optimizer', the property 'target' must be one of ['CPU', 'Ascend' ,'GPU'], "
                             "but got {}".format(value))

        if self._target == "CPU" and value in ('Ascend', 'GPU'):
            raise ValueError("For 'Optimizer', the property 'target' cannot be set to 'GPU' or 'Ascend' "
                             "in the 'CPU' environment.")

        if self._target == "Ascend" and value == 'GPU':
            raise ValueError("For 'Optimizer', the property 'target' cannot be set to 'GPU' "
                             "in the 'Ascend' environment.")

        if self._target == "GPU" and value == 'Ascend':
            raise ValueError("For 'Optimizer', the property 'target' cannot be set to 'Ascend' "
                             "in the 'GPU' environment.")

        self._is_device = (value != 'CPU')
        self._target = value

    def decay_weight(self, gradients):
        """
        Weight decay.

        An approach to reduce the overfitting of a deep learning neural network model. User-defined optimizers based
        on :class:`luojianet_ms.nn.Optimizer` can also call this interface to apply weight decay.

        Args:
            gradients (tuple[Tensor]):The gradients of network parameters, and have the same shape as the parameters.

        Returns:
            tuple[Tensor], The gradients after weight decay.
        """
        if self.exec_weight_decay:
            params = self.parameters
            if self.is_group:
                gradients = self.map_(F.partial(_apply_decay), self.weight_decay_tensor_tuple, self.decay_flags,
                                      params, gradients)
            else:
                gradients = self.map_(F.partial(_apply_decay, self.weight_decay_tensor), self.decay_flags,
                                      params, gradients)

        return gradients

    def gradients_centralization(self, gradients):
        """
        Gradients centralization.

        A method for optimizing convolutional layer parameters to improve the training speed of a deep learning neural
        network model. User-defined optimizers based on :class:`luojianet_ms.nn.Optimizer` can also call this interface to
        centralize gradients.

        Args:
            gradients (tuple[Tensor]): The gradients of network parameters, and have the same shape as the parameters.

        Returns:
            tuple[Tensor], The gradients after gradients centralization.
        """
        if self.is_group:
            gradients = self.map_(F.partial(_apply_grad_centralization), self.grad_centralization_flags, gradients)

        return gradients

    def scale_grad(self, gradients):
        """
        Restore gradients for mixed precision.

        User-defined optimizers based on :class:`luojianet_ms.nn.Optimizer` can also call this interface to restore
        gradients.

        Args:
            gradients (tuple[Tensor]): The gradients of network parameters, and have the same shape as the parameters.

        Returns:
            tuple[Tensor], The gradients after loss scale.

        """
        if self.need_scale:
            gradients = self.map_(F.partial(_grad_scale, self.reciprocal_scale), gradients)

        return gradients

    def _grad_sparse_indices_deduplicate(self, gradients):
        """ In the case of using big operators, deduplicate the 'indexes' in gradients."""
        if self._target != 'CPU' and self._unique:
            gradients = self.map_(F.partial(_indices_deduplicate), gradients)
        return gradients

    def _preprocess_weight_decay(self, weight_decay):
        """Check weight decay, and convert int to float."""
        if isinstance(weight_decay, (float, int)):
            weight_decay = float(weight_decay)
            validator.check_non_negative_float(weight_decay, "weight_decay", self.cls_name)
            return weight_decay
        raise TypeError("Weight decay should be int or float.")

    def _preprocess_grad_centralization(self, grad_centralization):
        if not isinstance(grad_centralization, bool):
            raise TypeError("For 'Optimizer', the 'gradients_centralization' should be bool type, "
                            "but got {}.".format(type(grad_centralization)))
        return grad_centralization

    def _preprocess_single_lr(self, learning_rate):
        """Check lr value, and convert lr to a float, a Tensor or a LearningRateSchedule."""
        if isinstance(learning_rate, (float, int)):
            learning_rate = float(learning_rate)
            validator.check_non_negative_float(learning_rate, "learning rate", self.cls_name)
            return learning_rate
        if isinstance(learning_rate, Tensor) and learning_rate.ndim == 0:
            return learning_rate

        self.dynamic_lr = True
        if isinstance(learning_rate, Iterable):
            return Tensor(np.array(list(learning_rate)).astype(np.float32))
        if isinstance(learning_rate, Tensor):
            if learning_rate.ndim > 1:
                raise ValueError(f"For 'Optimizer', if 'learning_rate' is Tensor type, then the dimension of it should "
                                 f"be 0 or 1, but got {learning_rate.ndim}.")
            if learning_rate.ndim == 1 and learning_rate.size < 2:
                logger.warning("If use `Tensor` type dynamic learning rate, please make sure that the number "
                               "of elements in the tensor is greater than 1.")
            return learning_rate
        if isinstance(learning_rate, LearningRateSchedule):
            return learning_rate
        raise TypeError("For 'Optimizer', 'learning_rate' should be int, float, Tensor, Iterable or "
                        "LearningRateSchedule, but got {}.".format(type(learning_rate)))

    def _build_single_lr(self, learning_rate, name):
        """Build learning rate value, convert learning rate to a Parameter or a LearningRateSchedule."""
        if isinstance(learning_rate, float):
            learning_rate = Parameter(Tensor(learning_rate, mstype.float32), name)
            if self.is_group_lr and self.dynamic_lr:
                learning_rate = _ConvertToCell(learning_rate)
            return learning_rate
        if isinstance(learning_rate, Tensor) and learning_rate.ndim == 0:
            learning_rate = Parameter(learning_rate, name)
            if self.is_group_lr and self.dynamic_lr:
                learning_rate = _ConvertToCell(learning_rate)
            return learning_rate
        if isinstance(learning_rate, Tensor) and learning_rate.ndim == 1:
            return _IteratorLearningRate(learning_rate, name)
        return learning_rate

    def _parameters_base_check(self, parameters, param_info):
        if parameters is None:
            raise ValueError(f"Optimizer {param_info} can not be None.")
        if not isinstance(parameters, Iterable):
            raise TypeError(f"Optimizer {param_info} must be Iterable.")
        parameters = list(parameters)

        if not parameters:
            raise ValueError(f"Optimizer got an empty {param_info} list.")
        return parameters

    def _check_group_params(self, parameters):
        """Check group params."""
        parse_keys = ['params', 'lr', 'weight_decay', 'order_params', 'grad_centralization']
        for group_param in parameters:
            invalid_key = list(filter(lambda x: x not in parse_keys, group_param.keys()))
            if invalid_key:
                raise KeyError(f'The key "{invalid_key}" cannot be recognized in group params.')

            if 'order_params' in group_param.keys():
                if len(group_param.keys()) > 1:
                    raise ValueError("The order params dict in group parameters should "
                                     "only include the 'order_params' key.")
                if not isinstance(group_param['order_params'], Iterable):
                    raise TypeError("The value of 'order_params' should be an Iterable type.")
                continue

            parameters = self._parameters_base_check(group_param['params'], "group `params`")
            if not all(isinstance(x, Parameter) for x in parameters):
                raise TypeError("The group `params` should be an iterator of Parameter type.")

    def _parse_group_params(self, parameters, learning_rate):
        """Parse group params."""
        self._check_group_params(parameters)
        if isinstance(learning_rate, Tensor) and learning_rate.ndim == 1:
            tensor_lr_length = learning_rate.size
        else:
            tensor_lr_length = 0

        for group_param in parameters:
            if 'order_params' in group_param.keys():
                if len(group_param.keys()) > 1:
                    raise ValueError("The order params dict in group parameters should "
                                     "only include the 'order_params' key.")
                if not isinstance(group_param['order_params'], Iterable):
                    raise TypeError("The value of 'order_params' should be an Iterable type.")
                self.is_group_params_ordered = True
                continue

            if 'lr' in group_param.keys():
                self.is_group_lr = True
                group_lr = self._preprocess_single_lr(group_param['lr'])

                if isinstance(group_lr, Tensor) and group_lr.ndim == 1:
                    group_lr_length = group_lr.size
                    if tensor_lr_length == 0:
                        tensor_lr_length = group_lr_length
                    elif group_lr_length != tensor_lr_length:
                        raise ValueError("The Tensor type dynamic learning rate in group should be the same size.")

    def _init_group_params(self, parameters, learning_rate, weight_decay, grad_centralization):
        """Initialize learning rate, weight decay or grad centralization in group params."""
        self._parse_group_params(parameters, learning_rate)
        default_lr = self._build_single_lr(learning_rate, 'learning_rate')

        params_store = []
        for group_num, group_param in enumerate(parameters):
            if 'order_params' in group_param.keys():
                ordered_parameters = group_param['order_params']
                continue

            self.group_params += group_param['params']

            if 'lr' in group_param.keys():
                lr_param_name = 'learning_rate_group_' + str(group_num)
                lr = self._preprocess_single_lr(group_param['lr'])
                lr = self._build_single_lr(lr, lr_param_name)
            else:
                lr = default_lr

            if 'weight_decay' in group_param.keys():
                cur_weight_decay = self._preprocess_weight_decay(group_param['weight_decay'])
                weight_decay_ = cur_weight_decay * self.loss_scale
            else:
                weight_decay_ = weight_decay * self.loss_scale

            if 'grad_centralization' in group_param.keys():
                self.grad_centralization = self._preprocess_grad_centralization(group_param['grad_centralization'])
                for param in group_param['params']:
                    validator.check_value_type("parameter", param, [Parameter], self.cls_name)
                    grad_centralization_ = self.grad_centralization
            else:
                grad_centralization_ = grad_centralization

            for key in group_param.keys():
                if key not in ('params', 'lr', 'weight_decay', 'grad_centralization'):
                    logger.warning(f"The optimizer cannot parse '{key}' when setting parameter groups.")

            for param in group_param['params']:
                validator.check_value_type("parameter", param, [Parameter], self.cls_name)
                if param.name in params_store:
                    raise RuntimeError(f"For 'Optimizer', the {param.name} parameter already exists, it does not "
                                       f"support repeated setting. Please check whether the optimizer parameter "
                                       f"has been set multiple times.")

                params_store.append(param.name)
                self.group_lr.append(lr)
                self.group_weight_decay.append(weight_decay_)
                self.group_grad_centralization.append(grad_centralization_)

        if self.is_group_params_ordered:
            self._order_and_adjust_group_params(ordered_parameters)

    def _order_and_adjust_group_params(self, ordered_parameters):
        """
        Order group parameter, learning rate, weight decay and grad centralization in group params.
        """
        params_length = len(self.group_params)
        if len(ordered_parameters) != len(self.group_params):
            raise ValueError(f"The value of 'order_params' should be same with all group parameters.")

        ordered_params = [None] * params_length
        ordered_learning_rate = [None] * params_length
        ordered_weight_decay = [None] * params_length
        ordered_grad_centralization = [None] * params_length
        params_name = [param.name for param in ordered_parameters]

        for param, lr, wd, gc in zip(self.group_params, self.group_lr, self.group_weight_decay,
                                     self.group_grad_centralization):
            index = params_name.index(param.name)
            ordered_params[index] = param
            ordered_learning_rate[index] = lr
            ordered_weight_decay[index] = wd
            ordered_grad_centralization[index] = gc

        self.group_params = ordered_params
        self.group_lr = ordered_learning_rate
        self.group_weight_decay = ordered_weight_decay
        self.group_grad_centralization = ordered_grad_centralization

    def get_lr(self):
        """
        The optimizer calls this interface to get the learning rate for the current step. User-defined optimizers based
        on :class:`luojianet_ms.nn.Optimizer` can also call this interface before updating the parameters.

        Returns:
            float, the learning rate of current step.
        """
        lr = self.learning_rate
        if self.dynamic_lr:
            if self.is_group_lr:
                lr = ()
                for learning_rate in self.learning_rate:
                    current_dynamic_lr = learning_rate(self.global_step)
                    lr += (current_dynamic_lr,)
            else:
                lr = self.learning_rate(self.global_step)

            self.assignadd(self.global_step, self.global_step_increase_tensor)
        return lr

    def get_lr_parameter(self, param):
        """
        When parameters is grouped and learning rate is different for each group. Get the learning rate of the specified
        `param`.

        Args:
            param (Union[Parameter, list[Parameter]]): The `Parameter` or list of `Parameter`.

        Returns:
            Parameter, single `Parameter` or `list[Parameter]` according to the input type. If learning rate is dynamic,
            `LearningRateSchedule` or `list[LearningRateSchedule]` that used to calculate the learning rate will be
            returned.

        Examples:
            >>> from luojianet_ms import nn
            >>> net = Net()
            >>> conv_params = list(filter(lambda x: 'conv' in x.name, net.trainable_params()))
            >>> no_conv_params = list(filter(lambda x: 'conv' not in x.name, net.trainable_params()))
            >>> group_params = [{'params': conv_params, 'lr': 0.05},
            ...                 {'params': no_conv_params, 'lr': 0.01}]
            >>> optim = nn.Momentum(group_params, learning_rate=0.1, momentum=0.9, weight_decay=0.0)
            >>> conv_lr = optim.get_lr_parameter(conv_params)
            >>> print(conv_lr[0].asnumpy())
            0.05
        """
        def get_lr_value(learning_rate):
            if isinstance(learning_rate, (_ConvertToCell, _IteratorLearningRate)):
                return learning_rate.learning_rate

            return learning_rate

        if isinstance(param, Parameter):
            param_list = [param]
        elif isinstance(param, list):
            param_list = param
        else:
            raise TypeError(f"For 'get_lr_parameter', the 'param' must be 'Parameter' or 'list' type, "
                            f"but got {type(param)}.")

        lr = []
        ids = [id(p) for p in self.parameters]
        for p in param_list:
            validator.check_value_type("parameter", p, [Parameter], self.cls_name)
            if id(p) not in ids:
                raise ValueError(f"For 'get_lr_parameter', the parameter {p.name} is not in optimizer, please check "
                                 f"whether the argument 'param' is correct.")
            if self.is_group_lr:
                index = ids.index(id(p))
                lr.append(get_lr_value(self.learning_rate[index]))
            else:
                lr.append(get_lr_value(self.learning_rate))

        return lr if isinstance(param, list) else lr[0]

    def _get_parameter_group_id(self):
        """
        Get the parameter partition group id, which is less than the number of devices.

        Returns:
            tuple, the group id tuple of parameters.
        """
        rank_list = ()
        count = 0
        for _ in range(self.param_length):
            rank_list = rank_list + (count,)
            count = count + 1
            if count == self.dev_num:
                count = 0
        return rank_list

    def broadcast_params(self, optim_result):
        """
        Apply Broadcast operations in the sequential order of parameter groups.

        Args:
            optim_result(bool): The results of updating parameters. This input is used to ensure that the parameters are
              updated before they are broadcast.
        Returns:
             bool, the status flag.
        """
        param_group = []
        key_group = []
        for _ in range(self.dev_num):
            param_group.append(F.make_tuple())
            key_group.append(F.make_tuple())
        for i in range(self.param_length):
            param_group[self.param_rank[i]] = param_group[self.param_rank[i]] + (self.parameters[i],)
            key = P.MakeRefKey(self.param_names[i])()
            key_group[self.param_rank[i]] = key_group[self.param_rank[i]] + (key,)
        new_param_group = []
        for root in range(self.dev_num):
            ops = P.Broadcast(root)
            if root > 0:
                param_group[root] = F.depend(param_group[root], new_param_group[root-1])
            else:
                param_group[root] = F.depend(param_group[root], optim_result)
            next_params = ops(param_group[root])
            new_param_group.append(next_params)
            for i in range(F.tuple_len(next_params)):
                F.assign(key_group[root][i], next_params[i])
        return new_param_group

    def call(self, *hyper_params):
        raise NotImplementedError


op_add = P.AddN()
op_gather = P.Gather()
op_mul = P.Mul()
op_gc = inner.Centralization()

_apply_decay = C.MultitypeFuncGraph("apply_decay")
_apply_grad_centralization = C.MultitypeFuncGraph("apply_grad_centralization")


@_apply_decay.register("Tensor", "Bool", "Tensor", "RowTensor")
def _tensor_apply_decay_with_sparse(weight_decay, if_apply, weight, gradient):
    """Get grad with weight_decay."""
    if if_apply:
        indices = gradient.indices
        values = op_add((op_gather(weight, indices, 0) * F.cast(weight_decay, F.dtype(weight)), gradient.values))
        shape = gradient.dense_shape
        return RowTensor(indices, values, shape)
    return gradient


@_apply_decay.register("Tensor", "Bool", "Tensor", "Tensor")
def _tensor_apply_decay(weight_decay, if_apply, weight, gradient):
    """Get grad with weight_decay."""
    if if_apply:
        return op_add((op_mul(weight, F.cast(weight_decay, F.dtype(weight))), gradient))
    return gradient


@_apply_grad_centralization.register("Bool", "RowTensor")
def _tensor_apply_grad_centralization_with_sparse(if_apply, gradient):
    """Get grad with grad_centralization."""
    if if_apply:
        indices = gradient.indices
        shape = gradient.dense_shape
        grad_shape = F.shape(gradient)
        axis = []
        for i in range(1, len(grad_shape)):
            axis.append(i)
        if len(axis) >= 1:
            if grad_shape[1] % 16 != 0:
                return gradient
            values = op_gc(gradient.values, axis)
            return RowTensor(indices, values, shape)
    return gradient


@_apply_grad_centralization.register("Bool", "Tensor")
def _tensor_apply_grad_centralization(if_apply, gradient):
    """Get grad with grad_centralization."""
    if if_apply:
        axis = []
        grad_shape = F.shape(gradient)
        for i in range(1, len(grad_shape)):
            axis.append(i)
        if len(axis) >= 1:
            if grad_shape[1] % 16 != 0:
                return gradient
            return op_gc(gradient, axis)
    return gradient


_grad_scale = C.MultitypeFuncGraph("grad_scale")
_indices_deduplicate = C.MultitypeFuncGraph("indices_deduplicate")


@_grad_scale.register("Number", "Tensor")
def tensor_grad_scale(scale, grad):
    """Get grad with scale."""
    if scale == 1.0:
        return grad
    return op_mul(grad, F.cast(scale, F.dtype(grad)))


@_grad_scale.register("Tensor", "Tensor")
def tensor_grad_scale_with_tensor(scale, grad):
    """Get grad with scale."""
    return op_mul(grad, F.cast(scale, F.dtype(grad)))


@_grad_scale.register("Tensor", "RowTensor")
def tensor_grad_scale_with_sparse(scale, grad):
    """Get grad with scale."""
    return RowTensor(grad.indices, grad.values * F.cast(scale, F.dtype(grad.values)), grad.dense_shape)


@_indices_deduplicate.register("RowTensor")
def rowtensor_deduplicate_indices_slices(grad):
    """Unique the indices and sums the 'values' corresponding to the duplicate indices."""
    indices = grad.indices
    values = grad.values

    unique_indices, index_position = P.Unique()(indices)
    summed_values = P.UnsortedSegmentSum()(values, index_position, P.DynamicShape()(unique_indices)[0])

    return RowTensor(unique_indices, summed_values, grad.dense_shape)


@_indices_deduplicate.register("Tensor")
def tensor_deduplicate_indice_slices(grad):
    """Return the input gradient directly in the dense sences."""
    return grad


class _ConvertToCell(LearningRateSchedule):
    """Inner api, convert learning rate of scalar to LearningRateSchedule."""
    def __init__(self, learning_rate):
        super(_ConvertToCell, self).__init__()
        if not isinstance(learning_rate, Parameter):
            raise TypeError("For 'Optimizer', the argument 'learning_rate' must be Parameter, "
                            "but got {}.".format(type(learning_rate)))
        self.learning_rate = learning_rate

    def call(self, global_step):
        return self.learning_rate + 1.0 - 1.0


class _IteratorLearningRate(LearningRateSchedule):
    """Inner api, convert learning rate of Tensor(list) to LearningRateSchedule."""
    def __init__(self, learning_rate, name):
        super(_IteratorLearningRate, self).__init__()
        if isinstance(learning_rate, Tensor):
            if learning_rate.ndim != 1:
                raise ValueError(f"For 'Optimizer', the dimension of the argument 'learning_rate' should "
                                 f"be 1, but got {learning_rate.ndim}.")
        else:
            raise TypeError("For 'Optimizer', the argument 'learning_rate' should be Tensor, "
                            "but got {}.".format(type(learning_rate)))

        self.learning_rate = Parameter(learning_rate, name)
        self.gather = P.Gather()

    def call(self, global_step):
        return self.gather(self.learning_rate, global_step, 0)
