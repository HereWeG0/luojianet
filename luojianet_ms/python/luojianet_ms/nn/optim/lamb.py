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
"""lamb"""
import numpy as np
from luojianet_ms import context
from luojianet_ms.common import dtype as mstype
from luojianet_ms.common.initializer import initializer
from luojianet_ms.ops import operations as P
from luojianet_ms.ops import composite as C
from luojianet_ms.ops import functional as F
from luojianet_ms.common.parameter import Parameter
from luojianet_ms.common.tensor import Tensor
from luojianet_ms._checkparam import Validator as validator
from luojianet_ms._checkparam import Rel
from .optimizer import Optimizer
from .optimizer import opt_init_args_register
from .. import layer


num_one = Tensor(np.ones([1]), mstype.float32)

_lamb_opt = C.MultitypeFuncGraph("lamb_opt")


@_lamb_opt.register("Tensor", "Tensor", "Tensor", "Tensor", "Tensor", "Number", "Tensor", "Tensor", "Tensor",
                    "Tensor", "Bool", "Bool")
def _update_run_op(beta1, beta2, eps, global_step, lr, weight_decay, param, m, v, gradient, decay_flag, optim_filter):
    """
    Update parameters.

    Args:
        beta1 (Tensor): The exponential decay rate for the 1st moment estimations. Should be in range (0.0, 1.0).
        beta2 (Tensor): The exponential decay rate for the 2nd moment estimations. Should be in range (0.0, 1.0).
        eps (Tensor): Term added to the denominator to improve numerical stability. Should be greater than 0.
        lr (Tensor): Learning rate.
        weight_decay (numbers.Number): Weight decay. Should be equal to or greater than 0.
        global_step (Tensor): Global step.
        param (Tensor): Parameters.
        m (Tensor): m value of parameters.
        v (Tensor): v value of parameters.
        gradient (Tensor): Gradient of parameters.
        decay_flag (bool): Specifies whether param update with weight decay.
        optim_filter(bool): Applies parameter update or not.

    Returns:
        Tensor, the new value of v after updating.
    """
    if optim_filter:
        op_mul = P.Mul()
        op_sqrt = P.Sqrt()
        op_rsqrt = P.Rsqrt()
        op_square = P.Square()
        op_cast = P.Cast()
        op_reshape = P.Reshape()
        op_shape = P.Shape()
        op_pow = P.Pow()
        op_norm = layer.Norm()
        op_select = P.Select()
        op_greater = P.Greater()
        op_fill = P.Fill()
        op_dtype = P.DType()

        param_fp32 = op_cast(param, mstype.float32)
        m_fp32 = op_cast(m, mstype.float32)
        v_fp32 = op_cast(v, mstype.float32)
        gradient_fp32 = op_cast(gradient, mstype.float32)

        next_m = op_mul(beta1, m_fp32) + op_mul(op_cast(num_one, mstype.float32) - beta1, gradient_fp32)

        next_v = op_mul(beta2, v_fp32) + op_mul(op_cast(num_one, mstype.float32) - beta2, op_square(gradient_fp32))

        next_mm = next_m / (op_cast(num_one, mstype.float32)
                            - op_pow(beta1, op_cast(global_step + num_one, mstype.float32)))
        next_vv = next_v / (op_cast(num_one, mstype.float32) -
                            op_pow(beta2, op_cast(global_step + num_one, mstype.float32)))
        w_norm = op_norm(param_fp32)
        g_norm = op_norm(gradient_fp32)

        g_norm_hat = op_norm(op_mul(next_mm, op_rsqrt(next_vv + eps)) + weight_decay * param_fp32)
        zeros = F.zeros_like(w_norm)
        ones = op_fill(op_dtype(w_norm), op_shape(w_norm), 1.0)
        trust_ratio = op_select(
            op_greater(w_norm, zeros),
            op_select(op_greater(g_norm, zeros), w_norm / g_norm_hat, ones),
            ones)
        tens = op_fill(op_dtype(trust_ratio), op_shape(trust_ratio), 10.0)
        trust_ratio = C.clip_by_value(trust_ratio, zeros, tens)
        update = next_mm / (op_sqrt(next_vv) + eps)

        if decay_flag:
            update = update + op_mul(weight_decay, param_fp32)

        update_with_lr = op_mul(op_mul(trust_ratio, lr), update)

        next_param = param_fp32 - op_reshape(update_with_lr, op_shape(param_fp32))

        next_param = F.depend(next_param, F.assign(param, op_cast(next_param, F.dtype(param))))
        next_param = F.depend(next_param, F.assign(m, op_cast(next_m, F.dtype(m))))
        next_param = F.depend(next_param, F.assign(v, op_cast(next_v, F.dtype(v))))

        return op_cast(next_param, F.dtype(param))
    return gradient

_lamb_opt_ascend = C.MultitypeFuncGraph("lamb_opt_ascend")


@_lamb_opt_ascend.register("Tensor", "Tensor", "Tensor", "Tensor", "Tensor", "Number", "Tensor", "Tensor", "Tensor",
                           "Tensor", "Bool", "Bool")
def _update_run_op_ascend(beta1, beta2, eps, global_step, lr, weight_decay, param, m, v, gradient, decay_flag,
                          optim_filter):
    """
    Update parameters function when device target is ascend.

    Args:
        beta1 (Tensor): The exponential decay rate for the 1st moment estimations. Should be in range (0.0, 1.0).
        beta2 (Tensor): The exponential decay rate for the 2nd moment estimations. Should be in range (0.0, 1.0).
        eps (Tensor): Term added to the denominator to improve numerical stability. Should be greater than 0.
        lr (Tensor): Learning rate.
        weight_decay (numbers.Number): Weight decay. Should be equal to or greater than 0.
        global_step (Tensor): Global step.
        param (Tensor): Parameters.
        m (Tensor): m value of parameters.
        v (Tensor): v value of parameters.
        gradient (Tensor): Gradient of parameters.
        decay_flag (bool): Specifies whether param update with weight decay.
        optim_filter(bool): Applies parameter update or not.

    Returns:
        Tensor, the new value of v after updating.
    """
    if optim_filter:
        op_cast = P.Cast()
        op_norm = layer.Norm()
        op_lamb_apply_optimizer_assign = P.LambApplyOptimizerAssign()
        op_lamb_apply_weight_assign = P.LambApplyWeightAssign()

        param_fp32 = op_cast(param, mstype.float32)
        gradient_fp32 = op_cast(gradient, mstype.float32)
        new_global_step = op_cast(global_step + num_one, mstype.float32)
        weight_decay_flag = op_cast(decay_flag, mstype.float32)

        update, _, _ = op_lamb_apply_optimizer_assign(gradient_fp32, v, m, param_fp32,
                                                      beta1, 1.0 - beta1, beta2, 1.0 - beta2, eps,
                                                      new_global_step, weight_decay_flag, weight_decay)
        w_norm = op_norm(param_fp32)
        g_norm = op_norm(update)
        update = F.depend(update, op_lamb_apply_weight_assign(w_norm, g_norm, lr, update, param))
        return update
    return gradient


def _check_param_value(beta1, beta2, eps, prim_name):
    validator.check_value_type("beta1", beta1, [float], prim_name)
    validator.check_value_type("beta2", beta2, [float], prim_name)
    validator.check_value_type("eps", eps, [float], prim_name)
    validator.check_float_range(beta1, 0.0, 1.0, Rel.INC_NEITHER, "beta1", prim_name)
    validator.check_float_range(beta2, 0.0, 1.0, Rel.INC_NEITHER, "beta2", prim_name)
    validator.check_positive_float(eps, "eps", prim_name)


class Lamb(Optimizer):
    r"""
    An optimizer that implements the Lamb(Layer-wise Adaptive Moments optimizer for Batching training) algorithm.

    LAMB is an optimization algorithm employing a layerwise adaptive large batch optimization technique.
    Refer to the paper `LARGE BATCH OPTIMIZATION FOR DEEP LEARNING: TRAINING BERT IN 76
    MINUTES <https://arxiv.org/abs/1904.00962>`_.

    The LAMB optimizer aims to increase the training batch size without reducing the accuracy,
    and it supports adaptive element-by-element update and accurate layered correction.

    The updating of parameters follows:

    ..  math::
        \begin{gather*}
        m_t = \beta_1 m_{t - 1}+ (1 - \beta_1)g_t\\
        v_t = \beta_2 v_{t - 1}  + (1 - \beta_2)g_t^2\\
        m_t = \frac{m_t}{\beta_1^t}\\
        v_t = \frac{v_t}{\beta_2^t}\\
        r_t = \frac{m_t}{\sqrt{v_t}+\epsilon}\\
        w_t = w_{t-1} -\eta_t \frac{\| w_{t-1} \|}{\| r_t + \lambda w_{t-1} \|} (r_t + \lambda w_{t-1})
        \end{gather*}

    where :math:`m` is the 1st moment, and :math:`v` the 2nd moment, :math:`\eta` the
    learning rate, :math:`\lambda` the LAMB weight decay rate.

    Note:
        There is usually no connection between a optimizer and mixed precision. But when `FixedLossScaleManager` is used
        and `drop_overflow_update` in `FixedLossScaleManager` is set to False, optimizer needs to set the 'loss_scale'.
        As this optimizer has no argument of `loss_scale`, so `loss_scale` needs to be processed by other means, refer
        document `LossScale <https://www.luojianet_ms.cn/docs/programming_guide/zh-CN/master/lossscale.html>`_ to process
        `loss_scale` correctly.

        If parameters are not grouped, the `weight_decay` in optimizer will be applied on the network parameters without
        'beta' or 'gamma' in their names. Users can group parameters to change the strategy of decaying weight. When
        parameters are grouped, each group can set `weight_decay`, if not, the `weight_decay` in optimizer will be
        applied.

    Args:
        params (Union[list[Parameter], list[dict]]): Must be list of `Parameter` or list of `dict`. When the
            `params` is a list of `dict`, the string "params", "lr", "weight_decay", "grad_centralization" and
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

        learning_rate (Union[float, int, Tensor, Iterable, LearningRateSchedule]):

            - float: The fixed learning rate value. Must be equal to or greater than 0.

            - int: The fixed learning rate value. Must be equal to or greater than 0. It will be converted to float.

            - Tensor: Its value should be a scalar or a 1-D vector. For scalar, fixed learning rate will be applied.
              For vector, learning rate is dynamic, then the i-th step will take the i-th value as the learning rate.

            - Iterable: Learning rate is dynamic. The i-th step will take the i-th value as the learning rate.

            - LearningRateSchedule: Learning rate is dynamic. During training, the optimizer calls the instance of
              LearningRateSchedule with step as the input to get the learning rate of current step.

        beta1 (float): The exponential decay rate for the 1st moment estimations. Default: 0.9.
            Should be in range (0.0, 1.0).
        beta2 (float): The exponential decay rate for the 2nd moment estimations. Default: 0.999.
            Should be in range (0.0, 1.0).
        eps (float): Term added to the denominator to improve numerical stability. Default: 1e-6.
            Should be greater than 0.
        weight_decay (float): Weight decay (L2 penalty). Default: 0.0. Should be equal to or greater than 0.

    Inputs:
        - **gradients** (tuple[Tensor]) - The gradients of `params`, the shape is the same as `params`.

    Outputs:
        tuple[bool], all elements are True.

    Raises:
        TypeError: If `learning_rate` is not one of int, float, Tensor, Iterable, LearningRateSchedule.
        TypeError: If element of `parameters` is neither Parameter nor dict.
        TypeError: If `beta1`, `beta2` or `eps` is not a float.
        TypeError: If `weight_decay` is neither float nor int.
        ValueError: If `eps` is less than or equal to 0.
        ValueError: If `beta1`, `beta2` is not in range (0.0, 1.0).
        ValueError: If `weight_decay` is less than 0.

    Supported Platforms:
        ``Ascend`` ``GPU``

    Examples:
        >>> net = Net()
        >>> #1) All parameters use the same learning rate and weight decay
        >>> optim = nn.Lamb(params=net.trainable_params(), learning_rate=0.1)
        >>>
        >>> #2) Use parameter groups and set different values
        >>> poly_decay_lr = learning_rate_schedule.PolynomialDecayLR(learning_rate=0.1, end_learning_rate=0.01,
        ...                                                    decay_steps=4, power = 0.5)
        >>> conv_params = list(filter(lambda x: 'conv' in x.name, net.trainable_params()))
        >>> no_conv_params = list(filter(lambda x: 'conv' not in x.name, net.trainable_params()))
        >>> group_params = [{'params': conv_params, 'weight_decay': 0.01, 'grad_centralization':True},
        ...                 {'params': no_conv_params, 'lr': poly_decay_lr},
        ...                 {'order_params': net.trainable_params(0.01)}]
        >>> optim = nn.Lamb(group_params, learning_rate=0.1, weight_decay=0.0)
        >>> # The conv_params's parameters will use default learning rate of 0.1 and weight decay of 0.01 and grad
        >>> # centralization of True.
        >>> # The no_conv_params's parameters will use dynamic learning rate of poly decay learning rate and default
        >>> # weight decay of 0.0 and grad centralization of False.
        >>> # The final parameters order in which the optimizer will be followed is the value of 'order_params'.
        >>>
        >>> loss = nn.SoftmaxCrossEntropyWithLogits()
        >>> model = Model(net, loss_fn=loss, optimizer=optim)
    """

    @opt_init_args_register
    def __init__(self, params, learning_rate, beta1=0.9, beta2=0.999, eps=1e-6, weight_decay=0.0):
        super(Lamb, self).__init__(learning_rate, params, weight_decay)
        _check_param_value(beta1, beta2, eps, self.cls_name)

        # turn them to scalar when me support scalar/tensor mix operations
        self.beta1 = Tensor(np.array([beta1]).astype(np.float32))
        self.beta2 = Tensor(np.array([beta2]).astype(np.float32))
        self.eps = Tensor(np.array([eps]).astype(np.float32))
        self.params = self.parameters
        self.moments1 = self.params.clone(prefix="lamb_m", init='zeros')
        self.moments2 = self.params.clone(prefix="lamb_v", init='zeros')

        if not self.dynamic_lr:
            self.global_step = Parameter(initializer(0, [1]), name='global_step')
            self.assignadd = P.AssignAdd()
        self.device_ascend = context.get_context("device_target") == "Ascend"

    def call(self, gradients):
        lr = self.get_lr()
        lamb_opt = _lamb_opt_ascend if self.device_ascend else _lamb_opt
        gradients = self.gradients_centralization(gradients)
        if self.is_group:
            if self.is_group_lr:
                optim_result = self.hyper_map(F.partial(lamb_opt, self.beta1, self.beta2, self.eps,
                                                        self.global_step),
                                              lr, self.weight_decay, self.params, self.moments1, self.moments2,
                                              gradients, self.decay_flags, self.optim_filter)
            else:
                optim_result = self.hyper_map(F.partial(lamb_opt, self.beta1, self.beta2, self.eps,
                                                        self.global_step, lr),
                                              self.weight_decay, self.params, self.moments1, self.moments2,
                                              gradients, self.decay_flags, self.optim_filter)
        else:
            optim_result = self.hyper_map(F.partial(lamb_opt, self.beta1, self.beta2, self.eps,
                                                    self.global_step, lr, self.weight_decay),
                                          self.params, self.moments1, self.moments2, gradients,
                                          self.decay_flags, self.optim_filter)

        if self.use_parallel:
            optim_result = F.depend(optim_result, self.broadcast_params(optim_result))

        if not self.dynamic_lr:
            optim_result = F.depend(optim_result, self.assignadd(self.global_step, 1))

        return optim_result
