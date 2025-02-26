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
"""test cases for Normal distribution"""
import numpy as np
from scipy import stats
import luojianet_ms.context as context
import luojianet_ms.nn as nn
import luojianet_ms.nn.probability.distribution as msd
from luojianet_ms import Tensor
from luojianet_ms import dtype

context.set_context(mode=context.GRAPH_MODE, device_target="Ascend")

class Prob(nn.Module):
    """
    Test class: probability of Normal distribution.
    """
    def __init__(self):
        super(Prob, self).__init__()
        self.n = msd.Normal(np.array([3.0]), np.array([[2.0], [4.0]]), dtype=dtype.float32)

    def call(self, x_):
        return self.n.prob(x_)

def test_pdf():
    """
    Test pdf.
    """
    norm_benchmark = stats.norm(np.array([3.0]), np.array([[2.0], [4.0]]))
    expect_pdf = norm_benchmark.pdf([1.0, 2.0]).astype(np.float32)
    pdf = Prob()
    output = pdf(Tensor([1.0, 2.0], dtype=dtype.float32))
    tol = 1e-6
    assert (np.abs(output.asnumpy() - expect_pdf) < tol).all()

class LogProb(nn.Module):
    """
    Test class: log probability of Normal distribution.
    """
    def __init__(self):
        super(LogProb, self).__init__()
        self.n = msd.Normal(np.array([3.0]), np.array([[2.0], [4.0]]), dtype=dtype.float32)

    def call(self, x_):
        return self.n.log_prob(x_)

def test_log_likelihood():
    """
    Test log_pdf.
    """
    norm_benchmark = stats.norm(np.array([3.0]), np.array([[2.0], [4.0]]))
    expect_logpdf = norm_benchmark.logpdf([1.0, 2.0]).astype(np.float32)
    logprob = LogProb()
    output = logprob(Tensor([1.0, 2.0], dtype=dtype.float32))
    tol = 1e-6
    assert (np.abs(output.asnumpy() - expect_logpdf) < tol).all()


class KL(nn.Module):
    """
    Test class: kl_loss of Normal distribution.
    """
    def __init__(self):
        super(KL, self).__init__()
        self.n = msd.Normal(np.array([3.0]), np.array([4.0]), dtype=dtype.float32)

    def call(self, x_, y_):
        return self.n.kl_loss('Normal', x_, y_)


def test_kl_loss():
    """
    Test kl_loss.
    """
    mean_a = np.array([3.0]).astype(np.float32)
    sd_a = np.array([4.0]).astype(np.float32)

    mean_b = np.array([1.0]).astype(np.float32)
    sd_b = np.array([1.0]).astype(np.float32)

    diff_log_scale = np.log(sd_a) - np.log(sd_b)
    squared_diff = np.square(mean_a / sd_b - mean_b / sd_b)
    expect_kl_loss = 0.5 * squared_diff + 0.5 * np.expm1(2 * diff_log_scale) - diff_log_scale

    kl_loss = KL()
    mean = Tensor(mean_b, dtype=dtype.float32)
    sd = Tensor(sd_b, dtype=dtype.float32)
    output = kl_loss(mean, sd)
    tol = 1e-6
    assert (np.abs(output.asnumpy() - expect_kl_loss) < tol).all()

class Basics(nn.Module):
    """
    Test class: mean/sd/mode of Normal distribution.
    """
    def __init__(self):
        super(Basics, self).__init__()
        self.n = msd.Normal(np.array([3.0]), np.array([2.0, 4.0]), dtype=dtype.float32)

    def call(self):
        return self.n.mean(), self.n.sd(), self.n.mode()

def test_basics():
    """
    Test mean/standard deviation/mode.
    """
    basics = Basics()
    mean, sd, mode = basics()
    expect_mean = [3.0, 3.0]
    expect_sd = [2.0, 4.0]
    tol = 1e-6
    assert (np.abs(mean.asnumpy() - expect_mean) < tol).all()
    assert (np.abs(mode.asnumpy() - expect_mean) < tol).all()
    assert (np.abs(sd.asnumpy() - expect_sd) < tol).all()

class Sampling(nn.Module):
    """
    Test class: sample of Normal distribution.
    """
    def __init__(self, shape, seed=0):
        super(Sampling, self).__init__()
        self.n = msd.Normal(np.array([3.0]), np.array([[2.0], [4.0]]), seed=seed, dtype=dtype.float32)
        self.shape = shape

    def call(self, mean=None, sd=None):
        return self.n.sample(self.shape, mean, sd)

def test_sample():
    """
    Test sample.
    """
    shape = (2, 3)
    seed = 10
    mean = Tensor([2.0], dtype=dtype.float32)
    sd = Tensor([2.0, 2.0, 2.0], dtype=dtype.float32)
    sample = Sampling(shape, seed=seed)
    output = sample(mean, sd)
    assert output.shape == (2, 3, 3)

class CDF(nn.Module):
    """
    Test class: cdf of Normal distribution.
    """
    def __init__(self):
        super(CDF, self).__init__()
        self.n = msd.Normal(np.array([3.0]), np.array([[2.0], [4.0]]), dtype=dtype.float32)

    def call(self, x_):
        return self.n.cdf(x_)


def test_cdf():
    """
    Test cdf.
    """
    norm_benchmark = stats.norm(np.array([3.0]), np.array([[2.0], [4.0]]))
    expect_cdf = norm_benchmark.cdf([1.0, 2.0]).astype(np.float32)
    cdf = CDF()
    output = cdf(Tensor([1.0, 2.0], dtype=dtype.float32))
    tol = 2e-5
    assert (np.abs(output.asnumpy() - expect_cdf) < tol).all()

class LogCDF(nn.Module):
    """
    Test class: log_cdf of Mormal distribution.
    """
    def __init__(self):
        super(LogCDF, self).__init__()
        self.n = msd.Normal(np.array([3.0]), np.array([[2.0], [4.0]]), dtype=dtype.float32)

    def call(self, x_):
        return self.n.log_cdf(x_)

def test_log_cdf():
    """
    Test log cdf.
    """
    norm_benchmark = stats.norm(np.array([3.0]), np.array([[2.0], [4.0]]))
    expect_logcdf = norm_benchmark.logcdf([1.0, 2.0]).astype(np.float32)
    logcdf = LogCDF()
    output = logcdf(Tensor([1.0, 2.0], dtype=dtype.float32))
    tol = 5e-5
    assert (np.abs(output.asnumpy() - expect_logcdf) < tol).all()

class SF(nn.Module):
    """
    Test class: survival function of Normal distribution.
    """
    def __init__(self):
        super(SF, self).__init__()
        self.n = msd.Normal(np.array([3.0]), np.array([[2.0], [4.0]]), dtype=dtype.float32)

    def call(self, x_):
        return self.n.survival_function(x_)

def test_survival():
    """
    Test log_survival.
    """
    norm_benchmark = stats.norm(np.array([3.0]), np.array([[2.0], [4.0]]))
    expect_survival = norm_benchmark.sf([1.0, 2.0]).astype(np.float32)
    survival_function = SF()
    output = survival_function(Tensor([1.0, 2.0], dtype=dtype.float32))
    tol = 2e-5
    assert (np.abs(output.asnumpy() - expect_survival) < tol).all()

class LogSF(nn.Module):
    """
    Test class: log survival function of Normal distribution.
    """
    def __init__(self):
        super(LogSF, self).__init__()
        self.n = msd.Normal(np.array([3.0]), np.array([[2.0], [4.0]]), dtype=dtype.float32)

    def call(self, x_):
        return self.n.log_survival(x_)

def test_log_survival():
    """
    Test log_survival.
    """
    norm_benchmark = stats.norm(np.array([3.0]), np.array([[2.0], [4.0]]))
    expect_log_survival = norm_benchmark.logsf([1.0, 2.0]).astype(np.float32)
    log_survival = LogSF()
    output = log_survival(Tensor([1.0, 2.0], dtype=dtype.float32))
    tol = 2e-5
    assert (np.abs(output.asnumpy() - expect_log_survival) < tol).all()

class EntropyH(nn.Module):
    """
    Test class: entropy of Normal distribution.
    """
    def __init__(self):
        super(EntropyH, self).__init__()
        self.n = msd.Normal(np.array([3.0]), np.array([[2.0], [4.0]]), dtype=dtype.float32)

    def call(self):
        return self.n.entropy()

def test_entropy():
    """
    Test entropy.
    """
    norm_benchmark = stats.norm(np.array([3.0]), np.array([[2.0], [4.0]]))
    expect_entropy = norm_benchmark.entropy().astype(np.float32)
    entropy = EntropyH()
    output = entropy()
    tol = 1e-6
    assert (np.abs(output.asnumpy() - expect_entropy) < tol).all()

class CrossEntropy(nn.Module):
    """
    Test class: cross entropy between Normal distributions.
    """
    def __init__(self):
        super(CrossEntropy, self).__init__()
        self.n = msd.Normal(np.array([3.0]), np.array([4.0]), dtype=dtype.float32)

    def call(self, x_, y_):
        entropy = self.n.entropy()
        kl_loss = self.n.kl_loss('Normal', x_, y_)
        h_sum_kl = entropy + kl_loss
        cross_entropy = self.n.cross_entropy('Normal', x_, y_)
        return h_sum_kl - cross_entropy

def test_cross_entropy():
    """
    Test cross_entropy.
    """
    cross_entropy = CrossEntropy()
    mean = Tensor([1.0], dtype=dtype.float32)
    sd = Tensor([1.0], dtype=dtype.float32)
    diff = cross_entropy(mean, sd)
    tol = 1e-6
    assert (np.abs(diff.asnumpy() - np.zeros(diff.shape)) < tol).all()

class Net(nn.Module):
    """
    Test class: expand single distribution instance to multiple graphs
    by specifying the attributes.
    """

    def __init__(self):
        super(Net, self).__init__()
        self.normal = msd.Normal(0., 1., dtype=dtype.float32)

    def call(self, x_, y_):
        kl = self.normal.kl_loss('Normal', x_, y_)
        prob = self.normal.prob(kl)
        return prob

def test_multiple_graphs():
    """
    Test multiple graphs case.
    """
    prob = Net()
    mean_a = np.array([0.0]).astype(np.float32)
    sd_a = np.array([1.0]).astype(np.float32)
    mean_b = np.array([1.0]).astype(np.float32)
    sd_b = np.array([1.0]).astype(np.float32)
    ans = prob(Tensor(mean_b), Tensor(sd_b))

    diff_log_scale = np.log(sd_a) - np.log(sd_b)
    squared_diff = np.square(mean_a / sd_b - mean_b / sd_b)
    expect_kl_loss = 0.5 * squared_diff + 0.5 * \
        np.expm1(2 * diff_log_scale) - diff_log_scale

    norm_benchmark = stats.norm(np.array([0.0]), np.array([1.0]))
    expect_prob = norm_benchmark.pdf(expect_kl_loss).astype(np.float32)

    tol = 1e-6
    assert (np.abs(ans.asnumpy() - expect_prob) < tol).all()
