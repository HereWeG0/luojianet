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

package com.luojianet_ms.lite;

import com.luojianet_ms.lite.LiteSession;
import com.luojianet_ms.lite.config.MSConfig;

public class TrainSession {
    static {
        System.loadLibrary("luojianet_ms-lite-train-jni");
    }

    public static LiteSession createTrainSession(String modelName, final MSConfig config, boolean trainMode) {
        LiteSession liteSession = new LiteSession();
        long sessionPtr = createTrainSession(modelName, config.getMSConfigPtr(), trainMode, 0);
        if (sessionPtr == 0) {
            return null;
        } else {
            liteSession.setSessionPtr(sessionPtr);
            return liteSession;
        }
    }

    private static native long createTrainSession(String fileName, long msContextPtr, boolean trainMode,
                                                  long msTrainCfgPtr);
}
