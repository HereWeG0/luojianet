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
# ==============================================================================
"""
Test KMnist dataset operators
"""

import os

import matplotlib.pyplot as plt
import numpy as np
import pytest

import luojianet_ms.dataset as ds
import luojianet_ms.dataset.vision.c_transforms as vision
from luojianet_ms import log as logger

DATA_DIR = "../data/dataset/testMnistData"


def load_kmnist(path):
    """
    Feature: load_kmnist.
    Description: load KMnistDataset.
    Expectation: get data of KMnistDataset.
    """
    labels_path = os.path.realpath(os.path.join(path, 't10k-labels-idx1-ubyte'))
    images_path = os.path.realpath(os.path.join(path, 't10k-images-idx3-ubyte'))
    with open(os.path.realpath(labels_path), 'rb') as lbpath:
        lbpath.read(8)
        labels = np.fromfile(lbpath, dtype=np.uint8)
    with open(os.path.realpath(images_path), 'rb') as imgpath:
        imgpath.read(16)
        images = np.fromfile(imgpath, dtype=np.uint8)
        images = images.reshape(-1, 28, 28, 1)
        images[images > 0] = 255  # Perform binarization to maintain consistency with our API
    return images, labels


def visualize_dataset(images, labels):
    """
    Feature: visualize_dataset.
    Description: visualize KMnistDataset.
    Expectation: plot images.
    """
    num_samples = len(images)
    for i in range(num_samples):
        plt.subplot(1, num_samples, i + 1)
        plt.imshow(images[i].squeeze(), cmap=plt.cm.gray)
        plt.title(labels[i])
    plt.show()


def test_kmnist_content_check():
    """
    Feature: test_kmnist_content_check.
    Description: validate KMnistDataset image readings.
    Expectation: get correct value.
    """
    logger.info("Test KMnistDataset Op with content check")
    data1 = ds.KMnistDataset(DATA_DIR, num_samples=100, shuffle=False)
    images, labels = load_kmnist(DATA_DIR)
    num_iter = 0
    # in this example, each dictionary has keys "image" and "label"
    image_list, label_list = [], []
    for i, data in enumerate(data1.create_dict_iterator(num_epochs=1, output_numpy=True)):
        image_list.append(data["image"])
        label_list.append("label {}".format(data["label"]))
        np.testing.assert_array_equal(data["image"], images[i])
        np.testing.assert_array_equal(data["label"], labels[i])
        num_iter += 1
    assert num_iter == 100


def test_kmnist_basic():
    """
    Feature: test_kmnist_basic.
    Description: test basic usage of KMnistDataset.
    Expectation: get correct data.
    """
    logger.info("Test KMnistDataset Op")

    # case 1: test loading whole dataset
    data1 = ds.KMnistDataset(DATA_DIR)
    num_iter1 = 0
    for _ in data1.create_dict_iterator(num_epochs=1):
        num_iter1 += 1
    assert num_iter1 == 10000

    # case 2: test num_samples
    data2 = ds.KMnistDataset(DATA_DIR, num_samples=500)
    num_iter2 = 0
    for _ in data2.create_dict_iterator(num_epochs=1):
        num_iter2 += 1
    assert num_iter2 == 500

    # case 3: test repeat
    data3 = ds.KMnistDataset(DATA_DIR, num_samples=200)
    data3 = data3.repeat(5)
    num_iter3 = 0
    for _ in data3.create_dict_iterator(num_epochs=1):
        num_iter3 += 1
    assert num_iter3 == 1000

    # case 4: test batch with drop_remainder=False
    data4 = ds.KMnistDataset(DATA_DIR, num_samples=100)
    assert data4.get_dataset_size() == 100
    assert data4.get_batch_size() == 1
    data4 = data4.batch(batch_size=7)  # drop_remainder is default to be False
    assert data4.get_dataset_size() == 15
    assert data4.get_batch_size() == 7
    num_iter4 = 0
    for _ in data4.create_dict_iterator(num_epochs=1):
        num_iter4 += 1
    assert num_iter4 == 15

    # case 5: test batch with drop_remainder=True
    data5 = ds.KMnistDataset(DATA_DIR, num_samples=100)
    assert data5.get_dataset_size() == 100
    assert data5.get_batch_size() == 1
    data5 = data5.batch(batch_size=7, drop_remainder=True)  # the rest of incomplete batch will be dropped
    assert data5.get_dataset_size() == 14
    assert data5.get_batch_size() == 7
    num_iter5 = 0
    for _ in data5.create_dict_iterator(num_epochs=1):
        num_iter5 += 1
    assert num_iter5 == 14

    # case 6: test get_col_names
    data6 = ds.KMnistDataset(DATA_DIR, "train", num_samples=10)
    assert data6.get_col_names() == ["image", "label"]

    #case 7: test batch
    data7 = ds.KMnistDataset(DATA_DIR, num_samples=200)
    data7 = data7.batch(100, drop_remainder=True)
    num_iter7 = 0
    for _ in data7.create_dict_iterator(num_epochs=1):
        num_iter7 += 1
    assert num_iter7 == 2


def test_kmnist_pk_sampler():
    """
    Feature: test_kmnist_pk_sampler.
    Description: test usage of KMnistDataset with PKSampler.
    Expectation: get correct data.
    """
    logger.info("Test KMnistDataset Op with PKSampler")
    golden = [0, 0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4,
              5, 5, 5, 6, 6, 6, 7, 7, 7, 8, 8, 8, 9, 9, 9]
    sampler = ds.PKSampler(3)
    data = ds.KMnistDataset(DATA_DIR, sampler=sampler)
    num_iter = 0
    label_list = []
    for item in data.create_dict_iterator(num_epochs=1, output_numpy=True):
        label_list.append(item["label"])
        num_iter += 1
    np.testing.assert_array_equal(golden, label_list)
    assert num_iter == 30


def test_kmnist_sequential_sampler():
    """
    Feature: test_kmnist_sequential_sampler.
    Description: test usage of KMnistDataset with SequentialSampler.
    Expectation: get correct data.
    """
    logger.info("Test KMnistDataset Op with SequentialSampler")
    num_samples = 50
    sampler = ds.SequentialSampler(num_samples=num_samples)
    data1 = ds.KMnistDataset(DATA_DIR, sampler=sampler)
    data2 = ds.KMnistDataset(DATA_DIR, shuffle=False, num_samples=num_samples)
    label_list1, label_list2 = [], []
    num_iter = 0
    for item1, item2 in zip(data1.create_dict_iterator(num_epochs=1), data2.create_dict_iterator(num_epochs=1)):
        label_list1.append(item1["label"].asnumpy())
        label_list2.append(item2["label"].asnumpy())
        num_iter += 1
    np.testing.assert_array_equal(label_list1, label_list2)
    assert num_iter == num_samples


def test_kmnist_exception():
    """
    Feature: test_kmnist_exception.
    Description: test error cases for KMnistDataset.
    Expectation: raise exception.
    """
    logger.info("Test error cases for KMnistDataset")
    error_msg_1 = "sampler and shuffle cannot be specified at the same time"
    with pytest.raises(RuntimeError, match=error_msg_1):
        ds.KMnistDataset(DATA_DIR, shuffle=False, sampler=ds.PKSampler(3))

    error_msg_2 = "sampler and sharding cannot be specified at the same time"
    with pytest.raises(RuntimeError, match=error_msg_2):
        ds.KMnistDataset(DATA_DIR, sampler=ds.PKSampler(3), num_shards=2, shard_id=0)

    error_msg_3 = "num_shards is specified and currently requires shard_id as well"
    with pytest.raises(RuntimeError, match=error_msg_3):
        ds.KMnistDataset(DATA_DIR, num_shards=10)

    error_msg_4 = "shard_id is specified but num_shards is not"
    with pytest.raises(RuntimeError, match=error_msg_4):
        ds.KMnistDataset(DATA_DIR, shard_id=0)

    error_msg_5 = "Input shard_id is not within the required interval"
    with pytest.raises(ValueError, match=error_msg_5):
        ds.KMnistDataset(DATA_DIR, num_shards=5, shard_id=-1)
    with pytest.raises(ValueError, match=error_msg_5):
        ds.KMnistDataset(DATA_DIR, num_shards=5, shard_id=5)
    with pytest.raises(ValueError, match=error_msg_5):
        ds.KMnistDataset(DATA_DIR, num_shards=2, shard_id=5)

    error_msg_6 = "num_parallel_workers exceeds"
    with pytest.raises(ValueError, match=error_msg_6):
        ds.KMnistDataset(DATA_DIR, shuffle=False, num_parallel_workers=0)
    with pytest.raises(ValueError, match=error_msg_6):
        ds.KMnistDataset(DATA_DIR, shuffle=False, num_parallel_workers=256)
    with pytest.raises(ValueError, match=error_msg_6):
        ds.KMnistDataset(DATA_DIR, shuffle=False, num_parallel_workers=-2)

    error_msg_7 = "Argument shard_id"
    with pytest.raises(TypeError, match=error_msg_7):
        ds.KMnistDataset(DATA_DIR, num_shards=2, shard_id="0")

    def exception_func(item):
        raise Exception("Error occur!")

    error_msg_8 = "The corresponding data files"
    with pytest.raises(RuntimeError, match=error_msg_8):
        data = ds.KMnistDataset(DATA_DIR)
        data = data.map(operations=exception_func, input_columns=["image"], num_parallel_workers=1)
        for _ in data.__iter__():
            pass
    with pytest.raises(RuntimeError, match=error_msg_8):
        data = ds.KMnistDataset(DATA_DIR)
        data = data.map(operations=vision.Decode(), input_columns=["image"], num_parallel_workers=1)
        data = data.map(operations=exception_func, input_columns=["image"], num_parallel_workers=1)
        for _ in data.__iter__():
            pass
    with pytest.raises(RuntimeError, match=error_msg_8):
        data = ds.KMnistDataset(DATA_DIR)
        data = data.map(operations=exception_func, input_columns=["label"], num_parallel_workers=1)
        for _ in data.__iter__():
            pass


def test_kmnist_visualize(plot=False):
    """
    Feature: test_kmnist_visualize.
    Description: visualize KMnistDataset results.
    Expectation: get correct data and plot them.
    """
    logger.info("Test KMnistDataset visualization")

    data1 = ds.KMnistDataset(DATA_DIR, num_samples=10, shuffle=False)
    num_iter = 0
    image_list, label_list = [], []
    for item in data1.create_dict_iterator(num_epochs=1, output_numpy=True):
        image = item["image"]
        label = item["label"]
        image_list.append(image)
        label_list.append("label {}".format(label))
        assert isinstance(image, np.ndarray)
        assert image.shape == (28, 28, 1)
        assert image.dtype == np.uint8
        assert label.dtype == np.uint32
        num_iter += 1
    assert num_iter == 10
    if plot:
        visualize_dataset(image_list, label_list)


def test_kmnist_usage():
    """
    Feature: test_kmnist_usage.
    Description: validate KMnistDataset image readings.
    Expectation: get correct data.
    """
    logger.info("Test KMnistDataset usage flag")

    def test_config(usage, kmnist_path=None):
        kmnist_path = DATA_DIR if kmnist_path is None else kmnist_path
        try:
            data = ds.KMnistDataset(kmnist_path, usage=usage, shuffle=False)
            num_rows = 0
            for _ in data.create_dict_iterator(num_epochs=1, output_numpy=True):
                num_rows += 1
        except (ValueError, TypeError, RuntimeError) as e:
            return str(e)
        return num_rows

    assert test_config("test") == 10000
    assert test_config("all") == 10000
    assert "KMnistDataset API can't read the data file (interface mismatch or no data found)" in test_config("train")
    assert "usage is not within the valid set of ['train', 'test', 'all']" in test_config("invalid")
    assert "Argument usage with value ['list'] is not of type [<class 'str'>]" in test_config(["list"])

    # change this directory to the folder that contains all kmnist files
    all_files_path = None
    # the following tests on the entire datasets
    if all_files_path is not None:
        assert test_config("train", all_files_path) == 60000
        assert test_config("test", all_files_path) == 10000
        assert test_config("all", all_files_path) == 70000
        assert ds.KMnistDataset(all_files_path, usage="train").get_dataset_size() == 60000
        assert ds.KMnistDataset(all_files_path, usage="test").get_dataset_size() == 10000
        assert ds.KMnistDataset(all_files_path, usage="all").get_dataset_size() == 70000


if __name__ == '__main__':
    test_kmnist_content_check()
    test_kmnist_basic()
    test_kmnist_pk_sampler()
    test_kmnist_sequential_sampler()
    test_kmnist_exception()
    test_kmnist_visualize(plot=True)
    test_kmnist_usage()
