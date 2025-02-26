/**
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

#include "gtest/gtest.h"
#include "base/dump_helper.h"
#include "base/schedule_tree_helper.h"
#include "poly/schedule_pass/reschedule.h"

namespace akg {

TEST(TestReschedule, TestCase1) {
  isl::schedule input_schedule;
  isl::schedule expect_schedule;
  std::tie(input_schedule, expect_schedule) = ScheduleTreeHelper("reschedule_case").Prepare();

  std::string dependences_str =
    "{ S_1[] -> S_32[]; S_17[] -> S_18[]; S_19[] -> S_22[]; S_8[] -> S_9[]; S_32[] -> S_33[]; S_13[] -> S_14[]; S_6[] "
    "-> S_16[]; S_33[] -> S_34[ax0, ax1] : 0 <= ax0 <= 1023 and 0 <= ax1 <= 1023; S_26[] -> S_27[]; S_24[] -> S_25[]; "
    "S_19[] -> S_20[]; S_23[] -> S_24[]; S_5[] -> S_7[]; S_28[] -> S_29[]; S_21[] -> S_24[]; S_4[] -> S_5[]; S_1[] -> "
    "S_13[]; S_12[] -> S_13[]; S_3[] -> S_4[]; S_10[] -> S_26[]; S_31[] -> S_32[]; S_16[] -> S_17[]; S_15[] -> S_16[]; "
    "S_18[] -> S_19[]; S_6[] -> S_7[]; S_1[] -> S_15[]; S_11[] -> S_29[]; S_7[] -> S_8[]; S_22[] -> S_23[]; S_10[] -> "
    "S_11[]; S_27[] -> S_28[]; S_0[] -> S_11[]; S_20[] -> S_25[]; S_1[] -> S_3[]; S_1[] -> S_5[]; S_25[] -> S_28[]; "
    "S_9[] -> S_10[]; S_34[ax0, ax1] -> S_35[ax0' = ax0, ax1' = ax1] : 0 <= ax0 <= 1023 and 0 <= ax1 <= 1023; S_0[] -> "
    "S_20[]; S_29[] -> S_31[]; S_14[] -> S_15[]; S_30[] -> S_31[]; S_2[] -> S_3[] }";

  std::string constraints_str =
    "{ domain: \"{ S_32[]; S_31[]; S_11[]; S_23[]; S_28[]; S_17[]; S_13[]; S_16[]; S_1[]; S_21[]; S_15[]; S_26[]; "
    "S_5[]; S_20[]; S_9[]; S_35[ax0, ax1] : 0 <= ax0 <= 1023 and 0 <= ax1 <= 1023; S_27[]; S_14[]; S_34[ax0, ax1] : 0 "
    "<= ax0 <= 1023 and 0 <= ax1 <= 1023; S_24[]; S_7[]; S_30[]; S_6[]; S_33[]; S_18[]; S_3[]; S_22[]; S_2[]; S_25[]; "
    "S_29[]; S_10[]; S_0[]; S_19[]; S_8[]; S_4[]; S_12[] }\", validity: \"{ S_1[] -> S_32[]; S_17[] -> S_18[]; S_19[] "
    "-> S_22[]; S_8[] -> S_9[]; S_32[] -> S_33[]; S_13[] -> S_14[]; S_6[] -> S_16[]; S_33[] -> S_34[ax0, ax1] : 0 <= "
    "ax0 <= 1023 and 0 <= ax1 <= 1023; S_26[] -> S_27[]; S_24[] -> S_25[]; S_19[] -> S_20[]; S_23[] -> S_24[]; S_5[] "
    "-> S_7[]; S_28[] -> S_29[]; S_21[] -> S_24[]; S_4[] -> S_5[]; S_1[] -> S_13[]; S_12[] -> S_13[]; S_3[] -> S_4[]; "
    "S_10[] -> S_26[]; S_31[] -> S_32[]; S_16[] -> S_17[]; S_15[] -> S_16[]; S_18[] -> S_19[]; S_6[] -> S_7[]; S_1[] "
    "-> S_15[]; S_11[] -> S_29[]; S_7[] -> S_8[]; S_22[] -> S_23[]; S_10[] -> S_11[]; S_27[] -> S_28[]; S_0[] -> "
    "S_11[]; S_20[] -> S_25[]; S_1[] -> S_3[]; S_1[] -> S_5[]; S_25[] -> S_28[]; S_9[] -> S_10[]; S_34[ax0, ax1] -> "
    "S_35[ax0' = ax0, ax1' = ax1] : 0 <= ax0 <= 1023 and 0 <= ax1 <= 1023; S_0[] -> S_20[]; S_29[] -> S_31[]; S_14[] "
    "-> S_15[]; S_30[] -> S_31[]; S_2[] -> S_3[] }\", proximity: \"{ S_1[] -> S_32[]; S_17[] -> S_18[]; S_19[] -> "
    "S_22[]; S_8[] -> S_9[]; S_32[] -> S_33[]; S_13[] -> S_14[]; S_6[] -> S_16[]; S_33[] -> S_34[ax0, ax1] : 0 <= ax0 "
    "<= 1023 and 0 <= ax1 <= 1023; S_26[] -> S_27[]; S_24[] -> S_25[]; S_19[] -> S_20[]; S_23[] -> S_24[]; S_5[] -> "
    "S_7[]; S_28[] -> S_29[]; S_21[] -> S_24[]; S_4[] -> S_5[]; S_1[] -> S_13[]; S_12[] -> S_13[]; S_3[] -> S_4[]; "
    "S_10[] -> S_26[]; S_31[] -> S_32[]; S_16[] -> S_17[]; S_15[] -> S_16[]; S_18[] -> S_19[]; S_6[] -> S_7[]; S_1[] "
    "-> S_15[]; S_11[] -> S_29[]; S_7[] -> S_8[]; S_22[] -> S_23[]; S_10[] -> S_11[]; S_27[] -> S_28[]; S_0[] -> "
    "S_11[]; S_20[] -> S_25[]; S_1[] -> S_3[]; S_1[] -> S_5[]; S_25[] -> S_28[]; S_9[] -> S_10[]; S_34[ax0, ax1] -> "
    "S_35[ax0' = ax0, ax1' = ax1] : 0 <= ax0 <= 1023 and 0 <= ax1 <= 1023; S_0[] -> S_20[]; S_29[] -> S_31[]; S_14[] "
    "-> S_15[]; S_30[] -> S_31[]; S_2[] -> S_3[] }\" }";

  // set reads
  std::string reads_str =
    "{ [S_26[] -> __poly_ref_60[]] -> "
    "T_multiply_T_multiply_T_multiply_T_minimum_T_maximum_T_add_T_subtract_tensor_T_broadcast_to_T_multiply_tensor["
    "arg0 = 0]; [S_35[ax0, ax1] -> __poly_ref_84[]] -> "
    "T_multiply_T_multiply_T_maximum_T_minimum_T_add_T_multiply_tensor_T_multiply_T_multiply_T_multiply_T_minimum_T_"
    "maximum_T_add_T_subtract_tensor_T_broadcast_to_T_multiply_tensor_T_multiply_T_add_T_multiply_tensor_T_multiply_T_"
    "multiply_T_multiply_T_minimum_T_maximum_T_add_T_subtract_tensor_T_broadcast_to_T_multiply_tensor_T_multiply_T_"
    "divide_input_0_input_1_T_subtract_T_multiply_T_multiply_T_multiply_T_minimum_T_maximum_T_add_T_subtract_tensor_T_"
    "broadcast_to_T_multiply_tensor_T_subtract_T_multiply_T_multiply_T_multiply_T_minimum_T_maximum_T_add_T_subtract_"
    "tensor_T_broadcast_to_T_multiply_tensor_input_14_input_16[arg0 = ax0, arg1 = ax1] : 0 <= ax0 <= 1023 and 0 <= ax1 "
    "<= 1023; [S_25[] -> __poly_ref_58[]] -> "
    "T_multiply_T_divide_input_0_input_1_T_subtract_T_multiply_T_multiply_T_multiply_T_minimum_T_maximum_T_add_T_"
    "subtract_tensor_T_broadcast_to_T_multiply_tensor[arg0 = 0]; [S_13[] -> __poly_ref_27[]] -> tensor_rename1[arg0 = "
    "0]; [S_14[] -> __poly_ref_30[]] -> T_subtract_tensor_T_broadcast_to_rename5[arg0 = 0]; [S_24[] -> "
    "__poly_ref_55[]] -> "
    "T_subtract_T_multiply_T_multiply_T_multiply_T_minimum_T_maximum_T_add_T_subtract_tensor_T_broadcast_to_T_multiply_"
    "tensor[arg0 = 0]; [S_31[] -> __poly_ref_71[]] -> "
    "T_add_T_multiply_tensor_T_multiply_T_multiply_T_multiply_T_minimum_T_maximum_T_add_T_subtract_tensor_T_broadcast_"
    "to_T_multiply_tensor_T_multiply_T_add_T_multiply_tensor_T_multiply_T_multiply_T_multiply_T_minimum_T_maximum_T_"
    "add_T_subtract_tensor_T_broadcast_to_T_multiply_tensor_T_multiply_T_divide_input_0_input_1_T_subtract_T_multiply_"
    "T_multiply_T_multiply_T_minimum_T_maximum_T_add_T_subtract_tensor_T_broadcast_to_T_multiply_tensor_T_subtract_T_"
    "multiply_T_multiply_T_multiply_T_minimum_T_maximum_T_add_T_subtract_tensor_T_broadcast_to_T_multiply_tensor[arg0 "
    "= 0]; [S_5[] -> __poly_ref_9[]] -> T_add_T_subtract_tensor_T_broadcast_to[arg0 = 0]; [S_22[] -> __poly_ref_50[]] "
    "-> "
    "T_multiply_T_multiply_T_multiply_T_minimum_T_maximum_T_add_T_subtract_tensor_T_broadcast_to_T_multiply_tensor_"
    "rename12[arg0 = 0]; [S_5[] -> __poly_ref_10[]] -> tensor_rename1[arg0 = 0]; [S_11[] -> __poly_ref_23[]] -> "
    "T_multiply_T_multiply_T_multiply_T_minimum_T_maximum_T_add_T_subtract_tensor_T_broadcast_to_T_multiply_tensor["
    "arg0 = 0]; [S_2[] -> __poly_ref_2[]] -> input_0[arg0 = 0]; [S_29[] -> __poly_ref_67[]] -> "
    "T_multiply_tensor_T_multiply_T_multiply_T_multiply_T_minimum_T_maximum_T_add_T_subtract_tensor_T_broadcast_to_T_"
    "multiply_tensor[arg0 = 0]; [S_9[] -> __poly_ref_18[]] -> "
    "T_multiply_T_minimum_T_maximum_T_add_T_subtract_tensor_T_broadcast_to_T_multiply_tensor[arg0 = 0]; [S_18[] -> "
    "__poly_ref_40[]] -> "
    "T_multiply_T_minimum_T_maximum_T_add_T_subtract_tensor_T_broadcast_to_T_multiply_tensor_rename10[arg0 = 0]; "
    "[S_21[] -> __poly_ref_47[]] -> input_0[arg0 = 0]; [S_27[] -> __poly_ref_62[]] -> "
    "T_subtract_T_multiply_T_multiply_T_multiply_T_minimum_T_maximum_T_add_T_subtract_tensor_T_broadcast_to_T_multiply_"
    "tensor_rename14_19[arg0 = 0]; [S_13[] -> __poly_ref_28[]] -> T_broadcast_to_rename4[arg0 = 0]; [S_15[] -> "
    "__poly_ref_32[]] -> T_add_T_subtract_tensor_T_broadcast_to_rename6[arg0 = 0]; [S_17[] -> __poly_ref_38[]] -> "
    "T_minimum_T_maximum_T_add_T_subtract_tensor_T_broadcast_to_T_multiply_tensor_rename9[arg0 = 0]; [S_28[] -> "
    "__poly_ref_64[]] -> "
    "T_add_T_multiply_tensor_T_multiply_T_multiply_T_multiply_T_minimum_T_maximum_T_add_T_subtract_tensor_T_broadcast_"
    "to_T_multiply_tensor_T_multiply_T_divide_input_0_input_1_T_subtract_T_multiply_T_multiply_T_multiply_T_minimum_T_"
    "maximum_T_add_T_subtract_tensor_T_broadcast_to_T_multiply_tensor[arg0 = 0]; [S_4[] -> __poly_ref_7[]] -> "
    "T_subtract_tensor_T_broadcast_to[arg0 = 0]; [S_31[] -> __poly_ref_72[]] -> "
    "T_minimum_T_add_T_multiply_tensor_T_multiply_T_multiply_T_multiply_T_minimum_T_maximum_T_add_T_subtract_tensor_T_"
    "broadcast_to_T_multiply_tensor_T_multiply_T_add_T_multiply_tensor_T_multiply_T_multiply_T_multiply_T_minimum_T_"
    "maximum_T_add_T_subtract_tensor_T_broadcast_to_T_multiply_tensor_T_multiply_T_divide_input_0_input_1_T_subtract_T_"
    "multiply_T_multiply_T_multiply_T_minimum_T_maximum_T_add_T_subtract_tensor_T_broadcast_to_T_multiply_tensor_T_"
    "subtract_T_multiply_T_multiply_T_multiply_T_minimum_T_maximum_T_add_T_subtract_tensor_T_broadcast_to_T_multiply_"
    "tensor_23[arg0 = 0]; [S_32[] -> __poly_ref_74[]] -> "
    "T_minimum_T_add_T_multiply_tensor_T_multiply_T_multiply_T_multiply_T_minimum_T_maximum_T_add_T_subtract_tensor_T_"
    "broadcast_to_T_multiply_tensor_T_multiply_T_add_T_multiply_tensor_T_multiply_T_multiply_T_multiply_T_minimum_T_"
    "maximum_T_add_T_subtract_tensor_T_broadcast_to_T_multiply_tensor_T_multiply_T_divide_input_0_input_1_T_subtract_T_"
    "multiply_T_multiply_T_multiply_T_minimum_T_maximum_T_add_T_subtract_tensor_T_broadcast_to_T_multiply_tensor_T_"
    "subtract_T_multiply_T_multiply_T_multiply_T_minimum_T_maximum_T_add_T_subtract_tensor_T_broadcast_to_T_multiply_"
    "tensor[arg0 = 0]; [S_33[] -> __poly_ref_77[]] -> "
    "T_maximum_T_minimum_T_add_T_multiply_tensor_T_multiply_T_multiply_T_multiply_T_minimum_T_maximum_T_add_T_subtract_"
    "tensor_T_broadcast_to_T_multiply_tensor_T_multiply_T_add_T_multiply_tensor_T_multiply_T_multiply_T_multiply_T_"
    "minimum_T_maximum_T_add_T_subtract_tensor_T_broadcast_to_T_multiply_tensor_T_multiply_T_divide_input_0_input_1_T_"
    "subtract_T_multiply_T_multiply_T_multiply_T_minimum_T_maximum_T_add_T_subtract_tensor_T_broadcast_to_T_multiply_"
    "tensor_T_subtract_T_multiply_T_multiply_T_multiply_T_minimum_T_maximum_T_add_T_subtract_tensor_T_broadcast_to_T_"
    "multiply_tensor[arg0 = 0]; [S_3[] -> __poly_ref_5[]] -> T_broadcast_to[arg0 = 0]; [S_33[] -> __poly_ref_78[]] -> "
    "input_14[arg0 = 0]; [S_21[] -> __poly_ref_48[]] -> input_1[arg0 = 0]; [S_11[] -> __poly_ref_22[]] -> tensor[arg0 "
    "= 0]; [S_7[] -> __poly_ref_14[]] -> "
    "T_minimum_T_maximum_T_add_T_subtract_tensor_T_broadcast_to_T_multiply_tensor_2[arg0 = 0]; [S_10[] -> "
    "__poly_ref_20[]] -> "
    "T_multiply_T_multiply_T_minimum_T_maximum_T_add_T_subtract_tensor_T_broadcast_to_T_multiply_tensor[arg0 = 0]; "
    "[S_25[] -> __poly_ref_57[]] -> "
    "T_multiply_tensor_T_multiply_T_multiply_T_multiply_T_minimum_T_maximum_T_add_T_subtract_tensor_T_broadcast_to_T_"
    "multiply_tensor_rename13[arg0 = 0]; [S_35[ax0, ax1] -> __poly_ref_83[]] -> input_17[arg0 = ax0, arg1 = ax1] : 0 "
    "<= ax0 <= 1023 and 0 <= ax1 <= 1023; [S_16[] -> __poly_ref_35[]] -> "
    "T_maximum_T_add_T_subtract_tensor_T_broadcast_to_T_multiply_tensor_rename8[arg0 = 0]; [S_23[] -> __poly_ref_52[]] "
    "-> "
    "T_subtract_T_multiply_T_multiply_T_multiply_T_minimum_T_maximum_T_add_T_subtract_tensor_T_broadcast_to_T_multiply_"
    "tensor_15[arg0 = 0]; [S_24[] -> __poly_ref_54[]] -> T_divide_input_0_input_1[arg0 = 0]; [S_32[] -> "
    "__poly_ref_75[]] -> tensor_rename1[arg0 = 0]; [S_16[] -> __poly_ref_36[]] -> "
    "T_minimum_T_maximum_T_add_T_subtract_tensor_T_broadcast_to_T_multiply_tensor_2[arg0 = 0]; [S_20[] -> "
    "__poly_ref_44[]] -> tensor[arg0 = 0]; [S_28[] -> __poly_ref_65[]] -> "
    "T_subtract_T_multiply_T_multiply_T_multiply_T_minimum_T_maximum_T_add_T_subtract_tensor_T_broadcast_to_T_multiply_"
    "tensor_rename14[arg0 = 0]; [S_20[] -> __poly_ref_45[]] -> "
    "T_multiply_T_multiply_T_multiply_T_minimum_T_maximum_T_add_T_subtract_tensor_T_broadcast_to_T_multiply_tensor_"
    "rename12[arg0 = 0]; [S_8[] -> __poly_ref_16[]] -> "
    "T_minimum_T_maximum_T_add_T_subtract_tensor_T_broadcast_to_T_multiply_tensor[arg0 = 0]; [S_34[ax0, ax1] -> "
    "__poly_ref_81[]] -> input_16[arg0 = ax0, arg1 = ax1] : 0 <= ax0 <= 1023 and 0 <= ax1 <= 1023; [S_34[ax0, ax1] -> "
    "__poly_ref_80[]] -> "
    "T_multiply_T_maximum_T_minimum_T_add_T_multiply_tensor_T_multiply_T_multiply_T_multiply_T_minimum_T_maximum_T_add_"
    "T_subtract_tensor_T_broadcast_to_T_multiply_tensor_T_multiply_T_add_T_multiply_tensor_T_multiply_T_multiply_T_"
    "multiply_T_minimum_T_maximum_T_add_T_subtract_tensor_T_broadcast_to_T_multiply_tensor_T_multiply_T_divide_input_0_"
    "input_1_T_subtract_T_multiply_T_multiply_T_multiply_T_minimum_T_maximum_T_add_T_subtract_tensor_T_broadcast_to_T_"
    "multiply_tensor_T_subtract_T_multiply_T_multiply_T_multiply_T_minimum_T_maximum_T_add_T_subtract_tensor_T_"
    "broadcast_to_T_multiply_tensor_input_14[arg0 = 0] : 0 <= ax0 <= 1023 and 0 <= ax1 <= 1023; [S_7[] -> "
    "__poly_ref_13[]] -> T_maximum_T_add_T_subtract_tensor_T_broadcast_to_T_multiply_tensor[arg0 = 0]; [S_3[] -> "
    "__poly_ref_4[]] -> tensor_rename1[arg0 = 0]; [S_15[] -> __poly_ref_33[]] -> tensor_rename1[arg0 = 0]; [S_19[] -> "
    "__poly_ref_42[]] -> "
    "T_multiply_T_multiply_T_minimum_T_maximum_T_add_T_subtract_tensor_T_broadcast_to_T_multiply_tensor_rename11[arg0 "
    "= 0]; [S_12[] -> __poly_ref_25[]] -> input_2[arg0 = 0]; [S_29[] -> __poly_ref_68[]] -> "
    "T_multiply_T_add_T_multiply_tensor_T_multiply_T_multiply_T_multiply_T_minimum_T_maximum_T_add_T_subtract_tensor_T_"
    "broadcast_to_T_multiply_tensor_T_multiply_T_divide_input_0_input_1_T_subtract_T_multiply_T_multiply_T_multiply_T_"
    "minimum_T_maximum_T_add_T_subtract_tensor_T_broadcast_to_T_multiply_tensor_T_subtract_T_multiply_T_multiply_T_"
    "multiply_T_minimum_T_maximum_T_add_T_subtract_tensor_T_broadcast_to_T_multiply_tensor[arg0 = 0] }";

  // set writes
  std::string writes_str =
    "{ [S_9[] -> __poly_ref_19[]] -> "
    "T_multiply_T_multiply_T_minimum_T_maximum_T_add_T_subtract_tensor_T_broadcast_to_T_multiply_tensor[arg0 = 0]; "
    "[S_28[] -> __poly_ref_66[]] -> "
    "T_multiply_T_add_T_multiply_tensor_T_multiply_T_multiply_T_multiply_T_minimum_T_maximum_T_add_T_subtract_tensor_T_"
    "broadcast_to_T_multiply_tensor_T_multiply_T_divide_input_0_input_1_T_subtract_T_multiply_T_multiply_T_multiply_T_"
    "minimum_T_maximum_T_add_T_subtract_tensor_T_broadcast_to_T_multiply_tensor_T_subtract_T_multiply_T_multiply_T_"
    "multiply_T_minimum_T_maximum_T_add_T_subtract_tensor_T_broadcast_to_T_multiply_tensor[arg0 = 0]; [S_20[] -> "
    "__poly_ref_46[]] -> "
    "T_multiply_tensor_T_multiply_T_multiply_T_multiply_T_minimum_T_maximum_T_add_T_subtract_tensor_T_broadcast_to_T_"
    "multiply_tensor_rename13[arg0 = 0]; [S_19[] -> __poly_ref_43[]] -> "
    "T_multiply_T_multiply_T_multiply_T_minimum_T_maximum_T_add_T_subtract_tensor_T_broadcast_to_T_multiply_tensor_"
    "rename12[arg0 = 0]; [S_24[] -> __poly_ref_56[]] -> "
    "T_multiply_T_divide_input_0_input_1_T_subtract_T_multiply_T_multiply_T_multiply_T_minimum_T_maximum_T_add_T_"
    "subtract_tensor_T_broadcast_to_T_multiply_tensor[arg0 = 0]; [S_31[] -> __poly_ref_73[]] -> "
    "T_minimum_T_add_T_multiply_tensor_T_multiply_T_multiply_T_multiply_T_minimum_T_maximum_T_add_T_subtract_tensor_T_"
    "broadcast_to_T_multiply_tensor_T_multiply_T_add_T_multiply_tensor_T_multiply_T_multiply_T_multiply_T_minimum_T_"
    "maximum_T_add_T_subtract_tensor_T_broadcast_to_T_multiply_tensor_T_multiply_T_divide_input_0_input_1_T_subtract_T_"
    "multiply_T_multiply_T_multiply_T_minimum_T_maximum_T_add_T_subtract_tensor_T_broadcast_to_T_multiply_tensor_T_"
    "subtract_T_multiply_T_multiply_T_multiply_T_minimum_T_maximum_T_add_T_subtract_tensor_T_broadcast_to_T_multiply_"
    "tensor[arg0 = 0]; [S_22[] -> __poly_ref_51[]] -> "
    "T_subtract_T_multiply_T_multiply_T_multiply_T_minimum_T_maximum_T_add_T_subtract_tensor_T_broadcast_to_T_multiply_"
    "tensor_15[arg0 = 0]; [S_16[] -> __poly_ref_37[]] -> "
    "T_minimum_T_maximum_T_add_T_subtract_tensor_T_broadcast_to_T_multiply_tensor_rename9[arg0 = 0]; [S_29[] -> "
    "__poly_ref_69[]] -> "
    "T_add_T_multiply_tensor_T_multiply_T_multiply_T_multiply_T_minimum_T_maximum_T_add_T_subtract_tensor_T_broadcast_"
    "to_T_multiply_tensor_T_multiply_T_add_T_multiply_tensor_T_multiply_T_multiply_T_multiply_T_minimum_T_maximum_T_"
    "add_T_subtract_tensor_T_broadcast_to_T_multiply_tensor_T_multiply_T_divide_input_0_input_1_T_subtract_T_multiply_"
    "T_multiply_T_multiply_T_minimum_T_maximum_T_add_T_subtract_tensor_T_broadcast_to_T_multiply_tensor_T_subtract_T_"
    "multiply_T_multiply_T_multiply_T_minimum_T_maximum_T_add_T_subtract_tensor_T_broadcast_to_T_multiply_tensor[arg0 "
    "= 0]; [S_26[] -> __poly_ref_61[]] -> "
    "T_subtract_T_multiply_T_multiply_T_multiply_T_minimum_T_maximum_T_add_T_subtract_tensor_T_broadcast_to_T_multiply_"
    "tensor_rename14_19[arg0 = 0]; [S_6[] -> __poly_ref_12[]] -> "
    "T_minimum_T_maximum_T_add_T_subtract_tensor_T_broadcast_to_T_multiply_tensor_2[arg0 = 0]; [S_5[] -> "
    "__poly_ref_11[]] -> T_maximum_T_add_T_subtract_tensor_T_broadcast_to_T_multiply_tensor[arg0 = 0]; [S_27[] -> "
    "__poly_ref_63[]] -> "
    "T_subtract_T_multiply_T_multiply_T_multiply_T_minimum_T_maximum_T_add_T_subtract_tensor_T_broadcast_to_T_multiply_"
    "tensor_rename14[arg0 = 0]; [S_21[] -> __poly_ref_49[]] -> T_divide_input_0_input_1[arg0 = 0]; [S_14[] -> "
    "__poly_ref_31[]] -> T_add_T_subtract_tensor_T_broadcast_to_rename6[arg0 = 0]; [S_18[] -> __poly_ref_41[]] -> "
    "T_multiply_T_multiply_T_minimum_T_maximum_T_add_T_subtract_tensor_T_broadcast_to_T_multiply_tensor_rename11[arg0 "
    "= 0]; [S_2[] -> __poly_ref_3[]] -> T_broadcast_to[arg0 = 0]; [S_23[] -> __poly_ref_53[]] -> "
    "T_subtract_T_multiply_T_multiply_T_multiply_T_minimum_T_maximum_T_add_T_subtract_tensor_T_broadcast_to_T_multiply_"
    "tensor[arg0 = 0]; [S_8[] -> __poly_ref_17[]] -> "
    "T_multiply_T_minimum_T_maximum_T_add_T_subtract_tensor_T_broadcast_to_T_multiply_tensor[arg0 = 0]; [S_1[] -> "
    "__poly_ref_1[]] -> tensor_rename1[arg0 = 0]; [S_12[] -> __poly_ref_26[]] -> T_broadcast_to_rename4[arg0 = 0]; "
    "[S_7[] -> __poly_ref_15[]] -> T_minimum_T_maximum_T_add_T_subtract_tensor_T_broadcast_to_T_multiply_tensor[arg0 = "
    "0]; [S_32[] -> __poly_ref_76[]] -> "
    "T_maximum_T_minimum_T_add_T_multiply_tensor_T_multiply_T_multiply_T_multiply_T_minimum_T_maximum_T_add_T_subtract_"
    "tensor_T_broadcast_to_T_multiply_tensor_T_multiply_T_add_T_multiply_tensor_T_multiply_T_multiply_T_multiply_T_"
    "minimum_T_maximum_T_add_T_subtract_tensor_T_broadcast_to_T_multiply_tensor_T_multiply_T_divide_input_0_input_1_T_"
    "subtract_T_multiply_T_multiply_T_multiply_T_minimum_T_maximum_T_add_T_subtract_tensor_T_broadcast_to_T_multiply_"
    "tensor_T_subtract_T_multiply_T_multiply_T_multiply_T_minimum_T_maximum_T_add_T_subtract_tensor_T_broadcast_to_T_"
    "multiply_tensor[arg0 = 0]; [S_10[] -> __poly_ref_21[]] -> "
    "T_multiply_T_multiply_T_multiply_T_minimum_T_maximum_T_add_T_subtract_tensor_T_broadcast_to_T_multiply_tensor["
    "arg0 = 0]; [S_4[] -> __poly_ref_8[]] -> T_add_T_subtract_tensor_T_broadcast_to[arg0 = 0]; [S_15[] -> "
    "__poly_ref_34[]] -> T_maximum_T_add_T_subtract_tensor_T_broadcast_to_T_multiply_tensor_rename8[arg0 = 0]; [S_17[] "
    "-> __poly_ref_39[]] -> "
    "T_multiply_T_minimum_T_maximum_T_add_T_subtract_tensor_T_broadcast_to_T_multiply_tensor_rename10[arg0 = 0]; "
    "[S_25[] -> __poly_ref_59[]] -> "
    "T_add_T_multiply_tensor_T_multiply_T_multiply_T_multiply_T_minimum_T_maximum_T_add_T_subtract_tensor_T_broadcast_"
    "to_T_multiply_tensor_T_multiply_T_divide_input_0_input_1_T_subtract_T_multiply_T_multiply_T_multiply_T_minimum_T_"
    "maximum_T_add_T_subtract_tensor_T_broadcast_to_T_multiply_tensor[arg0 = 0]; [S_11[] -> __poly_ref_24[]] -> "
    "T_multiply_tensor_T_multiply_T_multiply_T_multiply_T_minimum_T_maximum_T_add_T_subtract_tensor_T_broadcast_to_T_"
    "multiply_tensor[arg0 = 0]; [S_13[] -> __poly_ref_29[]] -> T_subtract_tensor_T_broadcast_to_rename5[arg0 = 0]; "
    "[S_34[ax0, ax1] -> __poly_ref_82[]] -> "
    "T_multiply_T_multiply_T_maximum_T_minimum_T_add_T_multiply_tensor_T_multiply_T_multiply_T_multiply_T_minimum_T_"
    "maximum_T_add_T_subtract_tensor_T_broadcast_to_T_multiply_tensor_T_multiply_T_add_T_multiply_tensor_T_multiply_T_"
    "multiply_T_multiply_T_minimum_T_maximum_T_add_T_subtract_tensor_T_broadcast_to_T_multiply_tensor_T_multiply_T_"
    "divide_input_0_input_1_T_subtract_T_multiply_T_multiply_T_multiply_T_minimum_T_maximum_T_add_T_subtract_tensor_T_"
    "broadcast_to_T_multiply_tensor_T_subtract_T_multiply_T_multiply_T_multiply_T_minimum_T_maximum_T_add_T_subtract_"
    "tensor_T_broadcast_to_T_multiply_tensor_input_14_input_16[arg0 = ax0, arg1 = ax1] : 0 <= ax0 <= 1023 and 0 <= ax1 "
    "<= 1023; [S_33[] -> __poly_ref_79[]] -> "
    "T_multiply_T_maximum_T_minimum_T_add_T_multiply_tensor_T_multiply_T_multiply_T_multiply_T_minimum_T_maximum_T_add_"
    "T_subtract_tensor_T_broadcast_to_T_multiply_tensor_T_multiply_T_add_T_multiply_tensor_T_multiply_T_multiply_T_"
    "multiply_T_minimum_T_maximum_T_add_T_subtract_tensor_T_broadcast_to_T_multiply_tensor_T_multiply_T_divide_input_0_"
    "input_1_T_subtract_T_multiply_T_multiply_T_multiply_T_minimum_T_maximum_T_add_T_subtract_tensor_T_broadcast_to_T_"
    "multiply_tensor_T_subtract_T_multiply_T_multiply_T_multiply_T_minimum_T_maximum_T_add_T_subtract_tensor_T_"
    "broadcast_to_T_multiply_tensor_input_14[arg0 = 0]; [S_35[ax0, ax1] -> __poly_ref_85[]] -> "
    "T_subtract_input_17_T_multiply_T_multiply_T_maximum_T_minimum_T_add_T_multiply_tensor_T_multiply_T_multiply_T_"
    "multiply_T_minimum_T_maximum_T_add_T_subtract_tensor_T_broadcast_to_T_multiply_tensor_T_multiply_T_add_T_multiply_"
    "tensor_T_multiply_T_multiply_T_multiply_T_minimum_T_maximum_T_add_T_subtract_tensor_T_broadcast_to_T_multiply_"
    "tensor_T_multiply_T_divide_input_0_input_1_T_subtract_T_multiply_T_multiply_T_multiply_T_minimum_T_maximum_T_add_"
    "T_subtract_tensor_T_broadcast_to_T_multiply_tensor_T_subtract_T_multiply_T_multiply_T_multiply_T_minimum_T_"
    "maximum_T_add_T_subtract_tensor_T_broadcast_to_T_multiply_tensor_input_14_input_16[arg0 = ax0, arg1 = ax1] : 0 <= "
    "ax0 <= 1023 and 0 <= ax1 <= 1023; [S_0[] -> __poly_ref_0[]] -> tensor[arg0 = 0]; [S_30[] -> __poly_ref_70[]] -> "
    "T_minimum_T_add_T_multiply_tensor_T_multiply_T_multiply_T_multiply_T_minimum_T_maximum_T_add_T_subtract_tensor_T_"
    "broadcast_to_T_multiply_tensor_T_multiply_T_add_T_multiply_tensor_T_multiply_T_multiply_T_multiply_T_minimum_T_"
    "maximum_T_add_T_subtract_tensor_T_broadcast_to_T_multiply_tensor_T_multiply_T_divide_input_0_input_1_T_subtract_T_"
    "multiply_T_multiply_T_multiply_T_minimum_T_maximum_T_add_T_subtract_tensor_T_broadcast_to_T_multiply_tensor_T_"
    "subtract_T_multiply_T_multiply_T_multiply_T_minimum_T_maximum_T_add_T_subtract_tensor_T_broadcast_to_T_multiply_"
    "tensor_23[arg0 = 0]; [S_3[] -> __poly_ref_6[]] -> T_subtract_tensor_T_broadcast_to[arg0 = 0] }";

  ir::poly::PassInfo pass_info;
  pass_info.constraints_ = isl::schedule_constraints(input_schedule.ctx().get(), constraints_str);
  pass_info.dependences_ = isl::union_map(input_schedule.ctx().get(), dependences_str);

  auto reads = isl::union_map(input_schedule.ctx().get(), reads_str);
  auto writes = isl::union_map(input_schedule.ctx().get(), writes_str);
  ir::poly::ScopInfo scop_info = ir::poly::ScopInfo(input_schedule.ctx());
  scop_info.analysis_result_.RecordReads(reads);
  scop_info.analysis_result_.RecordWrites(writes);

  auto output_schedule = ir::poly::Reschedule(scop_info, pass_info).Run(input_schedule);
  EXPECT_TRUE(SCH_EQUAL(output_schedule, expect_schedule));
}
}  // namespace akg
