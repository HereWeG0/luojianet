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
"""base process"""
import os
import time
import math
import copy
import numpy as np
from scipy import linalg as la
import luojianet_ms.nn as nn
from luojianet_ms.nn.optim import LARS
from luojianet_ms import log as logger
from luojianet_ms.common import Parameter
from luojianet_ms.communication.management import get_group_size
from luojianet_ms.parallel._utils import _get_global_rank
from luojianet_ms.train.serialization import load_checkpoint
from .less_batch_normalization import CommonHeadLastFN


__all__ = ["OptimizerProcess", "ParameterProcess", "get_local_pca_mat_path", "load_local_pca_mat"]


class OptimizerProcess:
    r"""
    Process optimizer for Boost. Currently, this class supports adding GC(grad centralization) tags
    and creating new optimizers.

    Args:
       opt (Module): Optimizer used.

    Examples:
        >>> from luojianet_ms import Tensor, Parameter, nn
        >>> import luojianet_ms.ops import ops
        >>> from luojianet_ms.boost import OptimizerProcess
        >>>
        >>> class Net(nn.Module):
        ...     def __init__(self, in_features, out_features):
        ...         super(Net, self).__init__()
        ...         self.weight = Parameter(Tensor(np.ones([in_features, out_features]).astype(np.float32)),
        ...                                 name='weight')
        ...         self.matmul = ops.MatMul()
        ...
        ...     def call(self, x):
        ...         output = self.matmul(x, self.weight)
        ...         return output
        ...
        >>> size, in_features, out_features = 16, 16, 10
        >>> network = Net(in_features, out_features)
        >>> optimizer = nn.Momentum(net.trainable_params(), learning_rate=0.1, momentum=0.9)
        >>> optimizer_process = OptimizerProcess(optimizer)
        >>> optimizer_process.add_grad_centralization(network)
        >>> optimizer = optimizer_process.generate_new_optimizer()
    """
    def __init__(self, opt):
        if isinstance(opt, LARS):
            self.is_lars = True
            self.opt_class = type(opt.opt)
            self.opt_init_args = opt.opt.init_args
            self.lars_init_args = opt.init_args
        else:
            self.is_lars = False
            self.opt_class = type(opt)
            self.opt_init_args = opt.init_args
        self.origin_params = opt.init_params["params"]

    def build_params_dict(self, network):
        r"""
        Build the parameter's dict of the network.

        Args:
            network (Module): The training network.
        """
        cells = network.cells_and_names()
        params_dict = {}
        for _, cell in cells:
            for par in cell.get_parameters(expand=False):
                params_dict[id(par)] = cell
        return params_dict

    def build_gc_params_group(self, params_dict, parameters):
        r"""
        Build the parameter's group with grad centralization.

        Args:
            params_dict (dict): The network's parameter dict.
            parameters (list): The network's parameter list.
        """
        group_params = []
        for group_param in parameters:
            if 'order_params' in group_param.keys():
                group_params.append(group_param)
                continue
            params_gc_value = []
            params_value = []
            for param in group_param['params']:
                if 'beta' not in param.name and 'gamma' not in param.name and 'bias' not in param.name:
                    param_cell = params_dict[id(param)]
                    if (isinstance(param_cell, nn.Conv2d) and param_cell.group > 1) or \
                        isinstance(param_cell, CommonHeadLastFN):
                        params_value.append(param)
                    else:
                        params_gc_value.append(param)
                else:
                    params_value.append(param)
            if params_gc_value:
                new_group_param = copy.deepcopy(group_param)
                new_group_param['params'] = params_gc_value
                new_group_param['grad_centralization'] = True
                group_params.append(new_group_param)
            if params_value:
                new_group_param = copy.deepcopy(group_param)
                new_group_param['params'] = params_value
                group_params.append(new_group_param)
        return group_params

    def add_grad_centralization(self, network):
        r"""
        Add gradient centralization.

        Args:
            network (Module): The training network.
        """
        params_dict = self.build_params_dict(network)

        parameters = self.origin_params
        if parameters is not None and not isinstance(parameters, list):
            parameters = list(parameters)

        if not parameters:
            raise ValueError("Optimizer got an empty parameter list.")

        if not isinstance(parameters[0], (dict, Parameter)):
            raise TypeError("Only a list of Parameter or dict can be supported.")

        if isinstance(parameters[0], Parameter):
            logger.warning("Only group parameters support gradient centralization.")
            return

        self.origin_params = self.build_gc_params_group(params_dict, parameters)

    def generate_new_optimizer(self):
        """Generate new optimizer."""
        if not self.is_lars:
            opt = self.opt_class(params=self.origin_params, **self.opt_init_args)
        else:
            opt = LARS(self.opt_class(params=self.origin_params, **self.opt_init_args), **self.lars_init_args)

        return opt


class ParameterProcess:
    r"""
    Process parameter for Boost. Currently, this class supports creating group parameters
    and automatically setting gradient segmentation point.

    Examples:
        >>> from luojianet_ms import Tensor, Parameter, nn
        >>> import luojianet_ms.ops as ops
        >>> from luojianet_ms.boost import OptimizerProcess
        >>>
        >>> class Net(nn.Module):
        ...     def __init__(self, in_features, out_features):
        ...         super(Net, self).__init__()
        ...         self.weight = Parameter(Tensor(np.ones([in_features, out_features]).astype(np.float32)),
        ...                                 name='weight')
        ...         self.weight2 = Parameter(Tensor(np.ones([in_features, out_features]).astype(np.float32)),
        ...                                 name='weight2')
        ...         self.matmul = ops.MatMul()
        ...         self.matmul2 = ops.MatMul()
        ...
        ...     def call(self, x):
        ...         output = self.matmul(x, self.weight)
        ...         output2 = self.matmul2(x, self.weight2)
        ...         return output + output2
        ...
        >>> size, in_features, out_features = 16, 16, 10
        >>> network = Net(in_features, out_features)
        >>> new_parameter = net.trainable_params()[:1]
        >>> parameter_process = ParameterProcess()
        >>> group_params = parameter_process.generate_group_params(new_parameter, net.trainable_params())
    """
    def __init__(self):
        self._parameter_indices = 1

    def assign_parameter_group(self, parameters, split_point=None):
        r"""
        Assign parameter group.

        Args:
            parameters (list): The network's parameter list.
            split_point (list): The gradient split point of this network. default: None.
        """
        if not isinstance(parameters, (list, tuple)) or not parameters:
            return parameters

        parameter_len = len(parameters)
        if split_point:
            split_parameter_index = split_point
        else:
            split_parameter_index = [parameter_len // 2]
        for i in range(parameter_len):
            if i in split_parameter_index:
                self._parameter_indices += 1
            parameters[i].comm_fusion = self._parameter_indices
        return parameters

    def generate_group_params(self, parameters, origin_params):
        r"""
        Generate group parameters.

        Args:
            parameters (list): The network's parameter list.
            origin_params (list): The network's origin parameter list.
        """
        origin_params_copy = origin_params
        if origin_params_copy is not None:
            if not isinstance(origin_params_copy, list):
                origin_params_copy = list(origin_params_copy)

        if not origin_params_copy:
            raise ValueError("Optimizer got an empty parameter list.")

        if not isinstance(origin_params_copy[0], (dict, Parameter)):
            raise TypeError("Only a list of Parameter or dict can be supported.")

        if isinstance(origin_params_copy[0], Parameter):
            group_params = [{"params": parameters}]
            return group_params

        group_params = []
        params_name = [param.name for param in parameters]
        new_params_count = copy.deepcopy(params_name)
        new_params_clone = {}
        max_key_number = 0
        for group_param in origin_params_copy:
            if 'order_params' in group_param.keys():
                new_group_param = copy.deepcopy(group_param)
                new_group_param['order_params'] = parameters
                group_params.append(new_group_param)
                continue
            params_value = []
            for param in group_param['params']:
                if param.name in params_name:
                    index = params_name.index(param.name)
                    params_value.append(parameters[index])
                    new_params_count.remove(param.name)
            new_group_param = copy.deepcopy(group_param)
            new_group_param['params'] = params_value
            group_params.append(new_group_param)
            if len(group_param.keys()) > max_key_number:
                max_key_number = len(group_param.keys())
                new_params_clone = copy.deepcopy(group_param)
        if new_params_count:
            params_value = []
            for param in new_params_count:
                index = params_name.index(param)
                params_value.append(parameters[index])
            if new_params_clone:
                new_params_clone['params'] = params_value
                group_params.append(new_params_clone)
            else:
                group_params.append({"params": params_value})
        return group_params


def get_local_pca_mat_path(weight_load_dir, pca_mat_path, n_component, device_number):
    """
    get local pca mat path.

    Args:
        weight_load_dir (str): The weight(ckpt) file directory to be load.
        pca_mat_path (str): the path to load pca mat. Default: None.
        n_component (int): pca component.
        device_number (int): device number.
    """
    if pca_mat_path is not None and os.path.exists(pca_mat_path) and os.path.isfile(pca_mat_path) and \
            pca_mat_path.endswith(".npy"):
        full_pca_mat_path = pca_mat_path
        pca_mat_exist = True

    else:
        if weight_load_dir is None or not os.path.exists(weight_load_dir) or not os.path.isdir(weight_load_dir):
            raise ValueError("The weight_load_dir: {} is None / not exists / not directory.".format(weight_load_dir))

        full_pca_mat_path = os.path.join(weight_load_dir, "pca_mat_temp.npy")
        pca_mat_exist = False

    rank = _get_global_rank()
    local_pca_mat_path = full_pca_mat_path[:-4] + "_rank_" + str(rank) + ".npy"
    if os.path.exists(local_pca_mat_path):
        os.remove(local_pca_mat_path)
    if rank % device_number != 0:
        return local_pca_mat_path

    if pca_mat_exist:
        pca_mat = np.load(full_pca_mat_path)
    else:
        data = _load_weights(weight_load_dir)
        pca_mat = _compute_pca_mat(data, n_component)
        np.save(full_pca_mat_path, pca_mat)
    _save_local_pca_mat(pca_mat, full_pca_mat_path, n_component)
    return local_pca_mat_path


def _load_weights(weight_load_dir):
    """
    load weights.

    Args:
        weight_load_dir (str): The weight(ckpt) file directory to be load.
    """
    param_mat = None
    weight_file_list = os.listdir(weight_load_dir)
    for file in weight_file_list:
        if not file.endswith('.ckpt'):
            continue
        file_path = os.path.join(weight_load_dir, file)
        param_dict = load_checkpoint(file_path)
        param = None
        for _, value in param_dict.items():
            if param is None:
                param = value.asnumpy().reshape((1, -1))
            else:
                param = np.hstack((param, value.asnumpy().reshape((1, -1))))
        if param_mat is None:
            param_mat = param
        else:
            param_mat = np.vstack((param_mat, param))
    return param_mat


def _compute_pca_mat(data, n_component):
    """
    compute pca mat.

    Args:
        data : array-like of shape (n_samples, n_features)
            Training data, where `n_samples` is the number of samples
            and `n_features` is the number of features.
        n_component (int): pca component.
    """
    if data.shape[0] < n_component:
        raise ValueError("The samples: {} is less than: n_component {}.".format(data.shape[0], n_component))
    mean = np.mean(data, axis=0)
    data -= mean
    u, _, v = la.svd(data, full_matrices=False)
    _, v = _svd_flip(u, v)
    components = v[:n_component]
    return components


def _svd_flip(u, v):
    """
    svd flip.

    Args:
        u (ndarray): the output of `linalg.svd`.
        v (ndarray): the output of `linalg.svd`.
    """
    max_abs_cols = np.argmax(np.abs(u), axis=0)
    signs = np.sign(u[max_abs_cols, range(u.shape[1])])
    u *= signs
    v *= signs[:, np.newaxis]
    return u, v


def _save_local_pca_mat(pca_mat, full_pca_mat_path, n_component):
    """
    save pca mat.

    Args:
        pca_mat (numpy.ndarray): pca mat to be saved.
        full_pca_mat_path (str): the path of full pca mat.
        n_component (int): pca component.
    """
    rank_size = get_group_size()
    local_dim = math.ceil(n_component / rank_size)
    for rank_id in range(rank_size):
        start_index = rank_id * local_dim
        end_index = (rank_id + 1) * local_dim
        pca_start_index = min(n_component, start_index)
        pca_end_index = min(n_component, end_index)
        p_local = np.zeros([local_dim + 1, pca_mat.shape[1]])
        if pca_start_index != pca_end_index:
            p_local[0: pca_end_index - pca_start_index, :] = pca_mat[pca_start_index: pca_end_index, :]
        local_pca_mat_path = full_pca_mat_path[:-4] + "_rank_" + str(rank_id) + ".npy"
        np.save(local_pca_mat_path, p_local)


def load_local_pca_mat(local_pca_mat_path, n_component):
    """
    load pca mat.

    Args:
        local_pca_mat_path (str): local pca mat file path.
        n_component (int): pca component.
    """
    rank_size = get_group_size()
    local_dim = math.ceil(n_component / rank_size)
    while True:
        if os.path.exists(local_pca_mat_path):
            break
        time.sleep(5)
    while True:
        pca_mat = np.load(local_pca_mat_path)
        if pca_mat.shape[0] == local_dim + 1:
            break
        time.sleep(5)
    pca_mat = pca_mat[:-1, :]
    return pca_mat
