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
Testing Autotune support in DE
"""
import numpy as np
import pytest
import luojianet_ms._c_dataengine as cde
import luojianet_ms.dataset as ds


# pylint: disable=unused-variable
@pytest.mark.forked
class TestAutotuneWithProfiler:
    def test_autotune_after_profiler_with_1_pipeline(self, capfd):
        """
        Feature: Autotuning with Profiler
        Description: Test Autotune enabled together with MD Profiler with a single pipeline
        Expectation: Enable MD Profiler and print appropriate warning logs when trying to enable Autotune
        """
        md_profiler = cde.GlobalContext.profiling_manager()
        md_profiler.init()
        md_profiler.start()
        ds.config.set_enable_autotune(True)
        source = [(np.array([x]),) for x in range(1024)]
        data1 = ds.GeneratorDataset(source, ["data"])
        data1 = data1.shuffle(64)
        data1 = data1.batch(32)
        itr1 = data1.create_dict_iterator(num_epochs=5)

        _, err = capfd.readouterr()
        assert "Cannot enable AutoTune for the current data pipeline as Dataset Profiling is already enabled for the " \
               "current data pipeline." in err
        # Uncomment the following two lines to see complete stdout and stderr output in pytest summary output
        # sys.stdout.write(_)
        # sys.stderr.write(err)

        md_profiler.stop()
        ds.config.set_enable_autotune(False)

    def test_autotune_after_profiler_with_2_pipeline(self, capfd):
        """
        Feature: Autotuning with Profiler
        Description: Test Autotune enabled together with MD Profiler with two pipelines
        Expectation: Enable MD Profiler for first tree and print appropriate warning log when trying to
        enable Autotune for the first tree. Print appropriate warning logs when trying to enable both MD Profiler
        and Autotune for second tree.
        """
        md_profiler = cde.GlobalContext.profiling_manager()
        md_profiler.init()
        md_profiler.start()
        ds.config.set_enable_autotune(True)
        source = [(np.array([x]),) for x in range(1024)]
        data1 = ds.GeneratorDataset(source, ["data"])
        data1 = data1.shuffle(64)
        data1 = data1.batch(32)
        itr1 = data1.create_dict_iterator(num_epochs=5)

        _, err = capfd.readouterr()
        assert "Cannot enable AutoTune for the current data pipeline as Dataset Profiling is already enabled for the " \
               "current data pipeline." in err
        # Uncomment the following two lines to see complete stdout and stderr output in pytest summary output
        # sys.stdout.write(_)
        # sys.stderr.write(err)

        itr2 = data1.create_dict_iterator(num_epochs=5)

        _, err = capfd.readouterr()
        assert "Dataset Profiling is already enabled for a different data pipeline." in err
        assert "Cannot enable AutoTune for the current data pipeline as Dataset Profiling is enabled for another data" \
               " pipeline." in err
        # Uncomment the following two lines to see complete stdout and stderr output in pytest summary output
        # sys.stdout.write(_)
        # sys.stderr.write(err)

        md_profiler.stop()
        ds.config.set_enable_autotune(True)

    @pytest.mark.skip(reason="close non-sink")
    def test_autotune_with_2_pipeline(self, capfd):
        """
        Feature: Autotuning
        Description: Test Autotune two pipelines
        Expectation: Enable MD Profiler and print appropriate warning logs
        when trying to enable Autotune for second tree.
        """
        ds.config.set_enable_autotune(True)
        source = [(np.array([x]),) for x in range(1024)]
        data1 = ds.GeneratorDataset(source, ["data"])
        data1 = data1.shuffle(64)
        data1 = data1.batch(32)
        itr1 = data1.create_dict_iterator(num_epochs=5)
        itr2 = data1.create_dict_iterator(num_epochs=5)

        _, err = capfd.readouterr()
        assert "Cannot enable AutoTune for the current data pipeline as it is already enabled for another data " \
               "pipeline." in err
        # Uncomment the following two lines to see complete stdout and stderr output in pytest summary output
        # sys.stdout.write(_)
        # sys.stderr.write(err)

        ds.config.set_enable_autotune(False)

    @pytest.mark.skip(reason="close non-sink")
    def test_delayed_autotune_with_2_pipeline(self, capfd):
        """
        Feature: Autotuning
        Description: Test delayed Autotune with two pipelines
        Expectation: Enable MD Profiler for second tree and no warnings logs should be printed.
        """
        source = [(np.array([x]),) for x in range(1024)]
        data1 = ds.GeneratorDataset(source, ["data"])
        data1 = data1.shuffle(64)
        data1 = data1.batch(32)
        itr1 = data1.create_dict_iterator(num_epochs=5)

        ds.config.set_enable_autotune(True)
        itr2 = data1.create_dict_iterator(num_epochs=5)
        ds.config.set_enable_autotune(False)

        _, err = capfd.readouterr()
        assert err == ''
        # Uncomment the following two lines to see complete stdout and stderr output in pytest summary output
        # sys.stdout.write(_)
        # sys.stderr.write(err)

    @pytest.mark.skip(reason="close non-sink")
    def test_delayed_start_autotune_with_3_pipeline(self, capfd):
        """
        Feature: Autotuning
        Description: Test delayed Autotune and early stop with three pipelines
        Expectation: Enable MD Profiler for second tree and no warnings logs should be printed.
        """
        source = [(np.array([x]),) for x in range(1024)]
        data1 = ds.GeneratorDataset(source, ["data"])
        data1 = data1.shuffle(64)
        data1 = data1.batch(32)
        itr1 = data1.create_dict_iterator(num_epochs=5)

        ds.config.set_enable_autotune(True)
        itr2 = data1.create_dict_iterator(num_epochs=5)
        ds.config.set_enable_autotune(False)

        itr3 = data1.create_dict_iterator(num_epochs=5)

        _, err = capfd.readouterr()
        assert err == ''
        # Uncomment the following two lines to see complete stdout and stderr output in pytest summary output
        # sys.stdout.write(_)
        # sys.stderr.write(err)

    @pytest.mark.skip(reason="close non-sink")
    def test_autotune_before_profiler(self):
        """
        Feature: Autotuning with Profiler
        Description: Test Autotune with Profiler when Profiler is Initialized after autotune
        Expectation: Initialization of Profiler should throw an error.
        """
        # enable AT for 1st tree
        # profiler init should throw an error
        source = [(np.array([x]),) for x in range(1024)]
        data1 = ds.GeneratorDataset(source, ["data"])
        data1 = data1.shuffle(64)
        data1 = data1.batch(32)
        ds.config.set_enable_autotune(True)
        itr1 = data1.create_dict_iterator(num_epochs=5)
        ds.config.set_enable_autotune(False)

        md_profiler = cde.GlobalContext.profiling_manager()
        with pytest.raises(RuntimeError) as excinfo:
            md_profiler.init()

        assert "Unexpected error. Stop MD Autotune before initializing the MD Profiler." in str(excinfo.value)

    def test_autotune_simple_pipeline(self):
        """
        Feature: Autotuning
        Description: test simple pipeline of autotune - Generator -> Shuffle -> Batch
        Expectation: pipeline runs successfully
        """
        ds.config.set_enable_autotune(True)

        source = [(np.array([x]),) for x in range(1024)]
        data1 = ds.GeneratorDataset(source, ["data"])
        data1 = data1.shuffle(64)
        data1 = data1.batch(32)

        itr = data1.create_dict_iterator(num_epochs=5)
        for _ in range(5):
            for _ in itr:
                pass

        ds.config.set_enable_autotune(False)

    def test_autotune_config(self):
        """
        Feature: Autotuning
        Description: test basic config of autotune
        Expectation: config can be set successfully
        """
        autotune_state = ds.config.get_enable_autotune()
        assert autotune_state is False

        ds.config.set_enable_autotune(False)
        autotune_state = ds.config.get_enable_autotune()
        assert autotune_state is False

        with pytest.raises(TypeError):
            ds.config.set_enable_autotune(1)

        autotune_interval = ds.config.get_autotune_interval()
        assert autotune_interval == 0

        ds.config.set_autotune_interval(200)
        autotune_interval = ds.config.get_autotune_interval()
        assert autotune_interval == 200

        with pytest.raises(TypeError):
            ds.config.set_autotune_interval(20.012)

        with pytest.raises(ValueError):
            ds.config.set_autotune_interval(-999)
