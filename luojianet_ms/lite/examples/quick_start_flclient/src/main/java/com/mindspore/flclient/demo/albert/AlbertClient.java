/*
 * Copyright 2021, 2022 LuoJiaNET Research and Development Group, Wuhan University
 * Copyright 2021, 2022 Huawei Technologies Co., Ltd
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.luojianet_ms.flclient.demo.albert;

import com.luojianet_ms.flclient.demo.common.ClassifierAccuracyCallback;
import com.luojianet_ms.flclient.demo.common.PredictCallback;
import com.luojianet_ms.flclient.model.Callback;
import com.luojianet_ms.flclient.model.Client;
import com.luojianet_ms.flclient.model.ClientManager;
import com.luojianet_ms.flclient.model.DataSet;
import com.luojianet_ms.flclient.model.LossCallback;
import com.luojianet_ms.flclient.model.RunType;
import com.luojianet_ms.lite.config.MSConfig;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.logging.Logger;

/**
 * Defining the albert client class.
 *
 * @since v1.0
 */
public class AlbertClient extends Client {
    private static final Logger LOGGER = Logger.getLogger(AlbertClient.class.toString());
    private static final int NUM_OF_CLASS = 4;
    private static final int MAX_SEQ_LEN = 8;

    static {
        ClientManager.registerClient(new AlbertClient());
    }

    @Override
    public List<Callback> initCallbacks(RunType runType, DataSet dataSet) {
        List<Callback> callbacks = new ArrayList<>();
        if (runType == RunType.TRAINMODE) {
            Callback lossCallback = new LossCallback(trainSession);
            callbacks.add(lossCallback);
        } else if (runType == RunType.EVALMODE) {
            if (dataSet instanceof AlbertDataSet) {
                Callback evalCallback = new ClassifierAccuracyCallback(trainSession, dataSet.batchSize, NUM_OF_CLASS,
                        ((AlbertDataSet) dataSet).getTargetLabels());
                callbacks.add(evalCallback);
            }
        } else {
            Callback inferCallback = new PredictCallback(trainSession, dataSet.batchSize, NUM_OF_CLASS);
            callbacks.add(inferCallback);
        }
        return callbacks;
    }

    @Override
    public Map<RunType, Integer> initDataSets(Map<RunType, List<String>> files) {
        Map<RunType, Integer> sampleCounts = new HashMap<>();
        List<String> trainFiles = files.getOrDefault(RunType.TRAINMODE, null);
        if (trainFiles != null) {
            DataSet trainDataSet = new AlbertDataSet(RunType.TRAINMODE, MAX_SEQ_LEN);
            trainDataSet.init(trainFiles);
            dataSets.put(RunType.TRAINMODE, trainDataSet);
            sampleCounts.put(RunType.TRAINMODE, trainDataSet.sampleSize);
        }
        List<String> evalFiles = files.getOrDefault(RunType.EVALMODE, null);
        if (evalFiles != null) {
            DataSet evalDataSet = new AlbertDataSet(RunType.EVALMODE, MAX_SEQ_LEN);
            evalDataSet.init(evalFiles);
            dataSets.put(RunType.EVALMODE, evalDataSet);
            sampleCounts.put(RunType.EVALMODE, evalDataSet.sampleSize);
        }
        List<String> inferFiles = files.getOrDefault(RunType.INFERMODE, null);
        if (inferFiles != null) {
            DataSet inferDataSet = new AlbertDataSet(RunType.INFERMODE, MAX_SEQ_LEN);
            inferDataSet.init(inferFiles);
            dataSets.put(RunType.INFERMODE, inferDataSet);
            sampleCounts.put(RunType.INFERMODE, inferDataSet.sampleSize);
        }
        return sampleCounts;
    }

    @Override
    public float getEvalAccuracy(List<Callback> evalCallbacks) {
        for (Callback callBack : evalCallbacks) {
            if (callBack instanceof ClassifierAccuracyCallback) {
                return ((ClassifierAccuracyCallback) callBack).getAccuracy();
            }
        }
        LOGGER.severe("don not find accuracy related callback");
        return Float.NaN;
    }

    @Override
    public List<Integer> getInferResult(List<Callback> inferCallbacks) {
        DataSet inferDataSet = dataSets.getOrDefault(RunType.INFERMODE, null);
        if (inferDataSet == null) {
            return new ArrayList<>();
        }
        for (Callback callBack : inferCallbacks) {
            if (callBack instanceof PredictCallback) {
                return ((PredictCallback) callBack).getPredictResults().subList(0, inferDataSet.sampleSize);
            }
        }
        LOGGER.severe("don not find accuracy related callback");
        return new ArrayList<>();
    }
}
