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

#include <pybind11/operators.h>
#include "backend/kernel_compiler/oplib/oplib.h"
#include "pipeline/jit/pipeline.h"
#include "frontend/operator/composite/composite.h"
#include "pipeline/pynative/pynative_execute.h"
#include "utils/symbolic.h"
#include "pybind_api/api_register.h"
#include "pipeline/jit/parse/python_adapter.h"
#ifndef ENABLE_SECURITY
#include "utils/summary/event_writer.h"
#endif
#include "utils/config_manager.h"
#include "utils/mpi/mpi_config.h"
#include "utils/ms_utils.h"
#include "frontend/parallel/context.h"
#include "frontend/parallel/costmodel_context.h"
#include "frontend/optimizer/ad/dfunctor.h"
#ifdef ENABLE_GPU_COLLECTIVE
#include "runtime/device/gpu/distribution/collective_init.h"
#else
#include "runtime/device/gpu/distribution/collective_fake_init.h"
#endif
#if ((defined ENABLE_CPU) && (!defined _WIN32))
#include "ps/util.h"
#endif
#include "ps/ps_context.h"

#include "pybind_api/gil_scoped_long_running.h"

namespace py = pybind11;

using EnvInstance = luojianet_ms::EnvInstance;
using GraphExecutorPy = luojianet_ms::pipeline::GraphExecutorPy;
using Pipeline = luojianet_ms::pipeline::Pipeline;
using PrimitivePy = luojianet_ms::PrimitivePy;
using MetaFuncGraph = luojianet_ms::MetaFuncGraph;
#ifndef ENABLE_SECURITY
using EventWriter = luojianet_ms::summary::EventWriter;
#endif  // ENABLE_SECURITY
using OpLib = luojianet_ms::kernel::OpLib;
using ParallelContext = luojianet_ms::parallel::ParallelContext;
using CostModelContext = luojianet_ms::parallel::CostModelContext;
using luojianet_ms::MsCtxParam;
using PSContext = luojianet_ms::ps::PSContext;

// Interface with python
PYBIND11_MODULE(_c_expression, m) {
  // The OMP_NUM_THREADS has no effect when set in backend, so set it here in advance.
  luojianet_ms::common::SetOMPThreadNum();

  m.doc() = "LuoJiaNet c plugin";

  auto fns = luojianet_ms::PybindDefineRegister::AllFuncs();
  for (auto &item : fns) {
    item.second(&m);
  }

  luojianet_ms::ScopedLongRunning::SetHook(std::make_unique<luojianet_ms::GilScopedLongRunningHook>());

  // Class Pipeline interface
  (void)py::class_<GraphExecutorPy, std::shared_ptr<GraphExecutorPy>>(m, "GraphExecutor_")
    .def_static("get_instance", &GraphExecutorPy::GetInstance, "Executor get_instance.")
    .def("__call__", &GraphExecutorPy::Run, py::arg("args"), py::arg("phase") = py::str(""), "Executor run function.")
    .def("del_net_res", &GraphExecutorPy::DelNetRes, py::arg("network_id") = py::set(), "Delete network resource.")
    .def("get_func_graph", &GraphExecutorPy::GetFuncGraph, py::arg("phase") = py::str(""), "Get graph pointer.")
    .def("get_func_graph_proto", &GraphExecutorPy::GetFuncGraphProto, py::arg("phase") = py::str(""),
         py::arg("type") = py::str("onnx_ir"), "Get graph proto string by specifying ir type.")
    .def("compile", &GraphExecutorPy::Compile, py::arg("obj"), py::arg("args"), py::arg("phase") = py::str(""),
         py::arg("use_vm") = py::bool_(false), "Compile obj by executor.")
    .def("updata_param_node_default_input", &GraphExecutorPy::UpdataParamNodeDefaultInput, py::arg("phase"),
         py::arg("params"), "Fetch the inputs of Conv or Matmul for quant export.")
    .def("get_parameter_layout", &GraphExecutorPy::GetParameterLayout, py::arg("phase") = py::str("train"),
         "Get Parameter Tensor Layout Dictionary.")
    .def("get_parallel_parameter_name_list", &GraphExecutorPy::GetParallelParameterNameList,
         py::arg("phase") = py::str("train"), "Get Parallel Parameter Name List.")
    .def("get_strategy", &GraphExecutorPy::GetCNodeStrategy, py::arg("phase") = py::str("train"),
         "Get CNode Strategy Dictionary.")
    .def("get_num_parallel_ops", &GraphExecutorPy::GetNumOpsInfo, py::arg("phase") = py::str("train"),
         "Get the number of parallel operators.")
    .def("get_allreduce_fusion", &GraphExecutorPy::GetAllreduceFusion, py::arg("phase") = py::str("train"),
         "Get Allreduce Fusion Dictionary.")
    .def("fetch_info_for_quant_export", &GraphExecutorPy::FetchInfoForQuantExport, py::arg("phase") = py::str("train"),
         "Fetch the inputs of Conv or Matmul for quant export.")
    .def("build_data_graph", &GraphExecutorPy::BuildGraph, py::arg("build_params"), py::arg("phase") = py::str("train"),
         py::arg("broadcast_params") = py::dict(), "Build data graph.")
    .def("has_compiled", &GraphExecutorPy::HasCompiled, py::arg("phase") = py::str(""), "Get if cell compiled.")
    .def("run_init_graph", &GraphExecutorPy::RunInitGraph, "Run init Graph.")
    .def("set_py_exe_path", &GraphExecutorPy::PyExePath, py::arg("py_exe_path") = py::str(""),
         "Set python executable path.")
    .def("set_kernel_build_server_dir", &GraphExecutorPy::KernelBuildServerDir,
         py::arg("kernel_build_server_dir") = py::str(""), "Set kernel build server directory path.")
    .def("set_queue_name", &GraphExecutorPy::set_queue_name, py::arg("queue_name") = py::str(""),
         "Set queue name for the graph loaded from compile cache.")
    .def("set_enable_tuple_broaden", &GraphExecutorPy::set_enable_tuple_broaden,
         py::arg("enable_tuple_broaden") = py::bool_(false), "Set tuple broaden enable.")
    .def("set_compile_cache_dep_files", &GraphExecutorPy::set_compile_cache_dep_files,
         py::arg("compile_cache_dep_files") = py::list(), "Set the compilation cache dependent files.")
    .def("set_weights_values", &GraphExecutorPy::set_weights_values, py::arg("weights") = py::dict(),
         "Set values of weights.")
    .def("get_optimize_graph_proto", &GraphExecutorPy::GetOptimizeGraphProto, py::arg("phase") = py::str(""),
         "Get the optimize graph proto string.")
    .def("set_jit_config", &GraphExecutorPy::SetJitConfig, py::arg("jit_config") = py::dict(), "Set the jit config.");

  (void)py::class_<EnvInstance, std::shared_ptr<EnvInstance>>(m, "EnvInstance_").def(py::init());

  (void)m.def("generate_arguments_key", &luojianet_ms::pipeline::GenerateArgumentsKey, "Generate unique key of argument.");
  (void)m.def("real_run_op", &luojianet_ms::pynative::RealRunOp, "Run op pynatively.");
  (void)m.def("reset_op_id", &luojianet_ms::pipeline::ResetOpId, "Reset Operator Id");
  (void)m.def("init_hccl", &luojianet_ms::pipeline::InitHccl, "Init Hccl");
  (void)m.def("finalize_hccl", &luojianet_ms::pipeline::FinalizeHccl, "Finalize Hccl");
  (void)m.def("get_hccl_rank_id", &luojianet_ms::pipeline::GetHcclRankId, "Get Hccl Rank Id");
  (void)m.def("get_hccl_rank_size", &luojianet_ms::pipeline::GetHcclRankSize, "Get Hccl Rank Size");
  (void)m.def("verify_inputs_signature", &luojianet_ms::pipeline::VerifyInputSignature, "Verify input signature.");
  (void)m.def("init_exec_dataset", &luojianet_ms::pipeline::InitExecDataset, py::arg("queue_name"), py::arg("size"),
              py::arg("batch_size"), py::arg("types"), py::arg("shapes"), py::arg("input_indexs"),
              py::arg("phase") = py::str("dataset"), py::arg("need_run") = py::bool_(true), "Init and exec dataset.");
  (void)m.def("_set_dataset_mode_config", &luojianet_ms::ConfigManager::SetDatasetModeConfig, "API for set dataset mode.");
  (void)m.def("init_pipeline", &luojianet_ms::pipeline::InitPipeline, "Init Pipeline.");

  (void)m.def("export_graph", &luojianet_ms::pipeline::ExportGraph, "Export Graph.");
  (void)m.def("load_mindir", &luojianet_ms::pipeline::LoadMindIR, py::arg("file_name"), py::arg("dec_key") = nullptr,
              py::arg("key_len") = py::int_(0), py::arg("dec_mode") = py::str("AES-GCM"), "Load model as Graph.");

  (void)py::class_<luojianet_ms::MpiConfig, std::shared_ptr<luojianet_ms::MpiConfig>>(m, "MpiConfig")
    .def_static("get_instance", &luojianet_ms::MpiConfig::GetInstance, "Get mpi config instance.")
    .def("get_enable_mpi", &luojianet_ms::MpiConfig::enable_mpi, "Get whether enable mpi.")
    .def("set_enable_mpi", &luojianet_ms::MpiConfig::set_enable_mpi, "Set whether to enable mpi.");

  (void)py::class_<ParallelContext, std::shared_ptr<ParallelContext>>(m, "AutoParallelContext")
    .def_static("get_instance", &ParallelContext::GetInstance, "Get auto parallel context instance.")
    .def("get_device_num", &ParallelContext::device_num, "Get device num.")
    .def("set_hccl_test_avaible", &ParallelContext::set_hccl_test_available, "Set hccl test available.")
    .def("set_device_num", &ParallelContext::set_device_num, "Set device num.")
    .def("get_device_num_is_set", &ParallelContext::device_num_is_set, "Get device num is set.")
    .def("set_fusion_threshold_mb", &ParallelContext::set_fusion_threshold_mb, "Set fusion threshold.")
    .def("fusion_threshold_mb", &ParallelContext::fusion_threshold_mb, "Get fusion threshold.")
    .def("set_fusion_mode", &ParallelContext::set_fusion_mode, "Get fusion mode.")
    .def("get_fusion_mode", &ParallelContext::get_fusion_mode, "Get fusion mode.")
    .def("get_global_rank", &ParallelContext::global_rank, "Get global rank.")
    .def("set_global_rank", &ParallelContext::set_global_rank, "Set global rank.")
    .def("get_grad_accumulation_shard", &ParallelContext::grad_accumulation_shard, "Get grad_accumulation_shard.")
    .def("set_grad_accumulation_shard", &ParallelContext::set_grad_accumulation_shard, "Set grad_accumulation_shard.")
    .def("get_global_rank_is_set", &ParallelContext::global_rank_is_set, "Get global rank is set.")
    .def("get_gradients_mean", &ParallelContext::gradients_mean, "Get mirror mean.")
    .def("set_gradients_mean", &ParallelContext::set_gradients_mean, "Set mirror mean.")
    .def("get_gradient_fp32_sync", &ParallelContext::gradient_fp32_sync, "Get cast before mirror.")
    .def("set_gradient_fp32_sync", &ParallelContext::set_gradient_fp32_sync, "Set cast before mirror.")
    .def("get_loss_repeated_mean", &ParallelContext::loss_repeated_mean, "Get loss repeated mean.")
    .def("set_loss_repeated_mean", &ParallelContext::set_loss_repeated_mean, "Set loss repeated mean.")
    .def("get_parallel_mode", &ParallelContext::parallel_mode, "Get parallel mode.")
    .def("set_parallel_mode", &ParallelContext::set_parallel_mode, "Set parallel mode.")
    .def("get_grad_accumulation_step", &ParallelContext::grad_accumulation_step, "Get grad accumulation step.")
    .def("set_grad_accumulation_step", &ParallelContext::set_grad_accumulation_step, "Set grad accumulation step.")
    .def("get_strategy_search_mode", &ParallelContext::strategy_search_mode, "Get strategy search mode.")
    .def("set_strategy_search_mode", &ParallelContext::set_strategy_search_mode, "Set strategy search mode.")
    .def("set_all_reduce_fusion_split_indices", &ParallelContext::SetAllReduceFusionSplitIndices,
         "Set all reduce fusion split indices.")
    .def("get_all_reduce_fusion_split_indices", &ParallelContext::GetAllReduceFusionSplitIndices,
         "Get all reduce fusion split indices.")
    .def("set_all_reduce_fusion_split_sizes", &ParallelContext::SetAllReduceFusionSplitSizes,
         "Set all reduce fusion split sizes.")
    .def("get_all_reduce_fusion_split_sizes", &ParallelContext::GetAllReduceFusionSplitSizes,
         "Get all reduce fusion split sizes.")
    .def("set_enable_all_reduce_fusion", &ParallelContext::set_enable_all_reduce_fusion,
         "Set enable/disable all reduce fusion.")
    .def("get_enable_all_reduce_fusion", &ParallelContext::enable_all_reduce_fusion,
         "Get enable/disable all reduce fusion.")
    .def("get_parameter_broadcast", &ParallelContext::parameter_broadcast, "Get parameter broadcast.")
    .def("get_parameter_broadcast_is_set", &ParallelContext::parameter_broadcast_is_set,
         "Get parameter broadcast is set.")
    .def("set_parameter_broadcast", &ParallelContext::set_parameter_broadcast, "Set parameter broadcast.")
    .def("set_strategy_ckpt_load_file", &ParallelContext::set_strategy_ckpt_load_file,
         "Set strategy checkpoint load file.")
    .def("set_strategy_ckpt_save_file", &ParallelContext::set_strategy_ckpt_save_file,
         "Set strategy checkpoint save file.")
    .def("get_strategy_ckpt_load_file", &ParallelContext::strategy_ckpt_load_file, "Get strategy checkpoint load file.")
    .def("get_strategy_ckpt_save_file", &ParallelContext::strategy_ckpt_save_file, "Get strategy checkpoint save file.")
    .def("set_group_ckpt_save_file", &ParallelContext::set_group_ckpt_save_file, "Set group checkpoint save file.")
    .def("set_pipeline_stage_split_num", &ParallelContext::set_pipeline_stage_split_num,
         "Set pipeline stage split num.")
    .def("get_pipeline_stage_split_num", &ParallelContext::pipeline_stage_split_num, "Get pipeline stage split num.")
    .def("set_full_batch", &ParallelContext::set_full_batch, "Set whether load full batch on each device.")
    .def("get_full_batch", &ParallelContext::full_batch, "Get whether load full batch on each device.")
    .def("set_dataset_strategy", &ParallelContext::set_dataset_strategy, "Set dataset sharding strategy.")
    .def("get_dataset_strategy", &ParallelContext::dataset_strategy, "Get dataset sharding strategy.")
    .def("set_enable_parallel_optimizer", &ParallelContext::set_enable_parallel_optimizer,
         "Set enable/disable parallel optimizer.")
    .def("get_enable_parallel_optimizer", &ParallelContext::enable_parallel_optimizer,
         "Get enable/disable parallel optimizer.")
    .def("set_communi_parallel_mode", &ParallelContext::set_communi_parallel_mode, "Set communication parallel mode.")
    .def("get_communi_parallel_mode", &ParallelContext::communi_parallel_mode, "Get communication parallel mode.")
    .def("set_optimizer_weight_shard_size", &ParallelContext::set_optimizer_weight_shard_size,
         "Set opt shard group size when not fully use parallel optimizer.")
    .def("get_optimizer_weight_shard_size", &ParallelContext::optimizer_weight_shard_size,
         "Get opt shard group size when not fully use parallel optimizer.")
    .def("set_optimizer_weight_shard_aggregated_save", &ParallelContext::set_optimizer_weight_shard_aggregated_save,
         "Set whether to integrated save weight shard when enable parallel optimizer.")
    .def("get_optimizer_weight_shard_aggregated_save", &ParallelContext::optimizer_weight_shard_aggregated_save,
         "Get whether to integrated save weight shard when enable parallel optimizer.")
    .def("set_enable_alltoall", &ParallelContext::set_enable_all2all, "Set the enabling AllToAll value.")
    .def("get_enable_alltoall", &ParallelContext::enable_all2all, "Get the enabling AllToAll value.")
    .def("set_sharding_propagation", &ParallelContext::set_sharding_propagation,
         "Set sharding strategy propagation value.")
    .def("get_sharding_propagation", &ParallelContext::sharding_propagation, "Get sharding strategy propagation value.")
    .def("reset", &ParallelContext::Reset, "Reset auto parallel context.");

  (void)py::class_<CostModelContext, std::shared_ptr<CostModelContext>>(m, "CostModelContext")
    .def_static("get_instance", &CostModelContext::GetInstance, "Get cost_model context instance.")
    .def("set_device_memory_capacity", &CostModelContext::set_device_memory_capacity,
         "Set the capacity of device memory.")
    .def("get_device_memory_capacity", &CostModelContext::device_memory_capacity, "Get the capacity of device memory.")
    .def("set_costmodel_alpha", &CostModelContext::set_costmodel_alpha,
         "Set the parameter cost_model_alpha of the DP algorithm.")
    .def("get_costmodel_alpha", &CostModelContext::costmodel_alpha,
         "Get the parameter cost_model_alpha of the DP algorithm.")
    .def("set_costmodel_beta", &CostModelContext::set_costmodel_beta,
         "Set the parameter cost_model_beta of the DP algorithm.")
    .def("get_costmodel_beta", &CostModelContext::costmodel_beta,
         "Get the parameter cost_model_beta of the DP algorithm.")
    .def("set_costmodel_gamma", &CostModelContext::set_costmodel_gamma,
         "Set the parameter cost_model_gamma of the DP algorithm")
    .def("get_costmodel_gamma", &CostModelContext::costmodel_gamma,
         "Get the parameter cost_model_gamma of the DP algorithm.")
    .def("set_costmodel_communi_threshold", &CostModelContext::set_costmodel_communi_threshold,
         "Set the parameter cost_model_communi_threshold of the DP algorithm.")
    .def("get_costmodel_communi_threshold", &CostModelContext::costmodel_communi_threshold,
         "Get the parameter cost_model_communi_threshold of the DP algorithm.")
    .def("set_costmodel_communi_const", &CostModelContext::set_costmodel_communi_const,
         "Set the parameter cost_model_communi_const of the DP algorithm.")
    .def("get_costmodel_communi_const", &CostModelContext::costmodel_communi_const,
         "Get the parameter cost_model_communi_const of the DP algorithm.")
    .def("set_costmodel_communi_bias", &CostModelContext::set_costmodel_communi_bias,
         "Set the parameter cost_model_communi_bias of the DP algorithm.")
    .def("get_costmodel_communi_bias", &CostModelContext::costmodel_communi_bias,
         "Get the parameter cost_model_communi_bias of the DP algorithm.")
    .def("set_multi_subgraphs", &CostModelContext::set_multi_subgraphs, "Set the parameter is_multi_subgraphs.")
    .def("get_multi_subgraphs", &CostModelContext::is_multi_subgraphs, "Get the parameter is_multi_subgraphs.")
    .def("set_run_phase", &CostModelContext::set_run_phase, "Set the flag run_phase.")
    .def("get_run_phase", &CostModelContext::run_phase, "Get the flag run_phase.")
    .def("set_costmodel_allreduce_fusion_algorithm", &CostModelContext::set_costmodel_allreduce_fusion_algorithm,
         "Set the parameter gradient AllReduce fusion algorithm.")
    .def("get_costmodel_allreduce_fusion_algorithm", &CostModelContext::costmodel_allreduce_fusion_algorithm,
         "Get the parameter gradient AllReduce fusion algorithm.")
    .def("set_costmodel_allreduce_fusion_times", &CostModelContext::set_costmodel_allreduce_fusion_times,
         "Set the parameter gradient AllReduce times.")
    .def("get_costmodel_allreduce_fusion_times", &CostModelContext::costmodel_allreduce_fusion_times,
         "Get the parameter gradient AllReduce times.")
    .def("set_costmodel_allreduce_fusion_tail_percent", &CostModelContext::set_costmodel_allreduce_fusion_tail_percent,
         "Set the parameter gradient AllReduce fusion tail percent.")
    .def("get_costmodel_allreduce_fusion_tail_percent", &CostModelContext::costmodel_allreduce_fusion_tail_percent,
         "Get the parameter gradient AllReduce fusion tail percent.")
    .def("set_costmodel_allreduce_fusion_tail_time", &CostModelContext::set_costmodel_allreduce_fusion_tail_time,
         "Set the parameter gradient AllReduce fusion tail time.")
    .def("get_costmodel_allreduce_fusion_tail_time", &CostModelContext::costmodel_allreduce_fusion_tail_time,
         "Get the parameter gradient AllReduce fusion tail time.")
    .def("set_costmodel_allreduce_fusion_allreduce_inherent_time",
         &CostModelContext::set_costmodel_allreduce_fusion_allreduce_inherent_time,
         "Set the parameter gradient AllReduce fusion allreduce inherent time.")
    .def("get_costmodel_allreduce_fusion_allreduce_inherent_time",
         &CostModelContext::costmodel_allreduce_fusion_allreduce_inherent_time,
         "Get the parameter gradient AllReduce fusion allreduce inherent time.")
    .def("set_costmodel_allreduce_fusion_allreduce_bandwidth",
         &CostModelContext::set_costmodel_allreduce_fusion_allreduce_bandwidth,
         "Set the parameter gradient AllReduce fusion allreduce bandwidth.")
    .def("get_costmodel_allreduce_fusion_allreduce_bandwidth",
         &CostModelContext::costmodel_allreduce_fusion_allreduce_bandwidth,
         "Get the parameter gradient AllReduce fusion allreduce bandwidth.")
    .def("set_costmodel_allreduce_fusion_computation_time_parameter",
         &CostModelContext::set_costmodel_allreduce_fusion_computation_time_parameter,
         "Set the parameter gradient AllReduce fusion computation time parameter.")
    .def("get_costmodel_allreduce_fusion_computation_time_parameter",
         &CostModelContext::costmodel_allreduce_fusion_computation_time_parameter,
         "Get the parameter gradient AllReduce fusion computation time parameter.")
    .def("set_tensor_slice_align_enable", &CostModelContext::set_tensor_slice_alignment_enable,
         "Set the parameter tensor_slice_align_enable in strategy generation.")
    .def("get_tensor_slice_align_enable", &CostModelContext::tensor_slice_alignment_enable,
         "Get the parameter tensor_slice_align_enable in strategy generation.")
    .def("set_tensor_slice_align_size", &CostModelContext::set_tensor_slice_alignment_size,
         "Set the parameter tensor_slice_size in strategy generation.")
    .def("get_tensor_slice_align_size", &CostModelContext::tensor_slice_alignment_size,
         "Get the parameter tensor_slice_size in strategy generation.")
    .def("set_fully_use_devices", &CostModelContext::set_fully_use_device,
         "Set the parameter fully_use_devices in the DP algorithm.")
    .def("get_fully_use_devices", &CostModelContext::fully_use_device,
         "Get the parameter fully_use_devices in the DP algorithm.")
    .def("set_elementwise_op_strategy_follow", &CostModelContext::set_elementwise_stra_follow,
         "Set the parameter elementwise_op_strategy_follow in the DP algorithm.")
    .def("get_elementwise_op_strategy_follow", &CostModelContext::elementwise_stra_follow,
         "Get the parameter elementwise_op_strategy_follow in the DP algorithm.")
    .def("set_dp_algo_enable_approxi", &CostModelContext::set_dp_algo_enable_approxi,
         "Set the flag whether enabling approximation in the DP algorithm.")
    .def("get_dp_algo_enable_approxi", &CostModelContext::dp_algo_enable_approxi,
         "Get the flag whether enabling approximation in the DP algorithm.")
    .def("set_dp_algo_approxi_epsilon", &CostModelContext::set_dp_algo_approxi_epsilon,
         "Set the epsilon which is used in the approximation of DP algorithm.")
    .def("get_dp_algo_approxi_epsilon", &CostModelContext::dp_algo_approxi_epsilon,
         "Get the epsilon which is used in the approximation of DP algorithm.")
    .def("set_dp_algo_single_loop", &CostModelContext::set_dp_algo_single_loop,
         "Set the flag of generating a single suite of OperatorInfos in for-loop.")
    .def("get_dp_algo_single_loop", &CostModelContext::dp_algo_single_loop,
         "Get the flag of whether or not generating a single suite of OperatorInfos in for-loop.")
    .def("reset_cost_model", &CostModelContext::ResetCostModel, "Reset the CostModelContext.")
    .def("reset_algo_parameters", &CostModelContext::ResetAlgoParameters, "Reset the AlgoParameters.");

  (void)py::module::import("atexit").attr("register")(py::cpp_function{[&]() -> void {
#ifdef ENABLE_MINDDATA
    MS_LOG(INFO) << "Start releasing dataset handles...";
    py::module iterators = py::module::import("luojianet_ms.dataset.engine.iterators");
    (void)iterators.attr("_cleanup")();
    MS_LOG(INFO) << "End release dataset handles.";
#endif
    // only in case that c++ calling python interface, ClearResAtexit should be called.
    if (luojianet_ms::parse::python_adapter::IsPythonEnv()) {
      luojianet_ms::pipeline::ClearResAtexit();
    }
  }});

#ifndef ENABLE_SECURITY
  (void)py::class_<EventWriter, std::shared_ptr<EventWriter>>(m, "EventWriter_")
    .def(py::init<const std::string &>())
    .def("GetFileName", &EventWriter::GetFileName, "Get the file name.")
    .def("Open", &EventWriter::Open, "Open the write file.")
    .def("Write", &EventWriter::Write, "Write the serialize event.")
    .def("EventCount", &EventWriter::GetWriteEventCount, "Write event count.")
    .def("Flush", &EventWriter::Flush, "Flush the event.")
    .def("Close", &EventWriter::Close, "Close the write.")
    .def("Shut", &EventWriter::Shut, "Final close the write.");
#endif  // ENABLE_SECURITY

  (void)py::class_<OpLib, std::shared_ptr<OpLib>>(m, "Oplib")
    .def(py::init())
    .def_static("reg_op", &OpLib::RegOp, "Register op info.");
#ifdef ENABLE_GPU_COLLECTIVE
  (void)m.def("init_gpu_collective", &luojianet_ms::device::gpu::CollectiveInitializer::InitCollective,
              "Init gpu collective communication mode.");
  (void)m.def("finalize_gpu_collective", &luojianet_ms::device::gpu::CollectiveInitializer::FinalizeCollective,
              "Finalize gpu collective communication mode.");
  (void)m.def("get_rank_id", &luojianet_ms::device::gpu::CollectiveInitializer::GetRankID,
              "Finalize gpu collective communication mode.");
  (void)m.def("get_rank_size", &luojianet_ms::device::gpu::CollectiveInitializer::GetRankSize,
              "Finalize gpu collective communication mode.");
#else
  (void)m.def("init_gpu_collective", &luojianet_ms::device::gpu::CollectiveFakeInitializer::InitCollective,
              "Init gpu collective communication mode.");
  (void)m.def("finalize_gpu_collective", &luojianet_ms::device::gpu::CollectiveFakeInitializer::FinalizeCollective,
              "Finalize gpu collective communication mode.");
  (void)m.def("get_rank_id", &luojianet_ms::device::gpu::CollectiveFakeInitializer::GetRankID,
              "Finalize gpu collective communication mode.");
  (void)m.def("get_rank_size", &luojianet_ms::device::gpu::CollectiveFakeInitializer::GetRankSize,
              "Finalize gpu collective communication mode.");
#endif

  (void)py::class_<PSContext, std::shared_ptr<PSContext>>(m, "PSContext")
    .def_static("get_instance", &PSContext::instance, "Get PS context instance.")
    .def("set_ps_enable", &PSContext::SetPSEnable, "Set PS mode enabled or disabled.")
    .def("is_ps_mode", &PSContext::is_ps_mode, "Get PS mode enable-disable status.")
    .def("reset", &PSContext::Reset, "Reset PS context attributes.")
    .def("is_worker", &PSContext::is_worker, "Get whether the role of this process is Worker.")
    .def("is_server", &PSContext::is_server, "Get whether the role of this process is PServer.")
    .def("is_scheduler", &PSContext::is_scheduler, "Get whether the role of this process is Scheduler.")
    .def("ps_rank_id", &PSContext::ps_rank_id, "Get Worker and PServer rank id.")
    .def("insert_hash_table_size", &PSContext::InsertHashTableSize, "Insert hash table size.")
    .def("reinsert_hash_table_size", &PSContext::ReInsertHashTableSize,
         "Insert hash table size with new parameter name.")
    .def("insert_weight_init_info", &PSContext::InsertWeightInitInfo, "Insert embedding table initialization seed.")
    .def("insert_accumu_init_info", &PSContext::InsertAccumuInitInfo, "Insert accumulation initialization value.")
    .def("clone_hash_table", &PSContext::CloneHashTable, "Clone a hash table.")
    .def("set_cache_enable", &PSContext::set_cache_enable, "Set ps mode cache enable or not.")
    .def("set_rank_id", &PSContext::set_rank_id, "Set rank id for worker on ps mode.")
    .def("set_server_mode", &PSContext::set_server_mode, "Set server mode.")
    .def("server_mode", &PSContext::server_mode, "Get server mode.")
    .def("set_ms_role", &PSContext::set_ms_role, "Set role for this process.")
    .def("ms_role", &PSContext::ms_role, "Get role for this process.")
    .def("set_worker_num", &PSContext::set_worker_num, "Set worker number.")
    .def("worker_num", &PSContext::worker_num, "Get worker number.")
    .def("set_server_num", &PSContext::set_server_num, "Set server number.")
    .def("server_num", &PSContext::server_num, "Get server number.")
    .def("set_scheduler_ip", &PSContext::set_scheduler_ip, "Set scheduler ip.")
    .def("scheduler_ip", &PSContext::scheduler_ip, "Get scheduler ip.")
    .def("set_scheduler_port", &PSContext::set_scheduler_port, "Set scheduler port.")
    .def("scheduler_port", &PSContext::scheduler_port, "Get scheduler port.")
    .def("set_fl_server_port", &PSContext::set_fl_server_port, "Set federated learning server port.")
    .def("fl_server_port", &PSContext::fl_server_port, "Get federated learning server port.")
    .def("set_fl_client_enable", &PSContext::set_fl_client_enable, "Set federated learning client.")
    .def("fl_client_enable", &PSContext::fl_client_enable, "Get federated learning client.")
    .def("set_start_fl_job_threshold", &PSContext::set_start_fl_job_threshold,
         "Set threshold count for startFLJob round.")
    .def("start_fl_job_threshold", &PSContext::start_fl_job_threshold, "Get threshold count for startFLJob round.")
    .def("set_start_fl_job_time_window", &PSContext::set_start_fl_job_time_window,
         "Set time window for startFLJob round.")
    .def("start_fl_job_time_window", &PSContext::start_fl_job_time_window, "Get time window for startFLJob round.")
    .def("set_update_model_ratio", &PSContext::set_update_model_ratio,
         "Set threshold count ratio for updateModel round.")
    .def("update_model_ratio", &PSContext::update_model_ratio, "Get threshold count ratio for updateModel round.")
    .def("set_update_model_time_window", &PSContext::set_update_model_time_window,
         "Set time window for updateModel round.")
    .def("update_model_time_window", &PSContext::update_model_time_window, "Get time window for updateModel round.")
    .def("set_share_secrets_ratio", &PSContext::set_share_secrets_ratio,
         "Set threshold count ratio for share secrets round.")
    .def("share_secrets_ratio", &PSContext::share_secrets_ratio, "Get threshold count ratio for share secrets round.")
    .def("set_cipher_time_window", &PSContext::set_cipher_time_window, "Set time window for each cipher round.")
    .def("cipher_time_window", &PSContext::cipher_time_window, "Get time window for cipher rounds.")
    .def("set_reconstruct_secrets_threshold", &PSContext::set_reconstruct_secrets_threshold,
         "Set threshold count for reconstruct secrets round.")
    .def("reconstruct_secrets_threshold", &PSContext::reconstruct_secrets_threshold,
         "Get threshold count for reconstruct secrets round.")
    .def("set_fl_name", &PSContext::set_fl_name, "Set federated learning name.")
    .def("fl_name", &PSContext::fl_name, "Get federated learning name.")
    .def("set_fl_iteration_num", &PSContext::set_fl_iteration_num, "Set federated learning iteration number.")
    .def("fl_iteration_num", &PSContext::fl_iteration_num, "Get federated learning iteration number.")
    .def("set_client_epoch_num", &PSContext::set_client_epoch_num, "Set federated learning client epoch number.")
    .def("client_epoch_num", &PSContext::client_epoch_num, "Get federated learning client epoch number.")
    .def("set_client_batch_size", &PSContext::set_client_batch_size, "Set federated learning client batch size.")
    .def("client_batch_size", &PSContext::client_batch_size, "Get federated learning client batch size.")
    .def("set_client_learning_rate", &PSContext::set_client_learning_rate,
         "Set federated learning client learning rate.")
    .def("client_learning_rate", &PSContext::client_learning_rate,
         "Get worker's standalone training step number before communicating with server.")
    .def("set_worker_step_num_per_iteration", &PSContext::set_worker_step_num_per_iteration,
         "Set worker's standalone training step number before communicating with server..")
    .def("worker_step_num_per_iteration", &PSContext::worker_step_num_per_iteration,
         "Get federated learning client learning rate.")
    .def("set_secure_aggregation", &PSContext::set_secure_aggregation,
         "Set federated learning client using secure aggregation.")
    .def("set_dp_eps", &PSContext::set_dp_eps, "Set dp epsilon for federated learning secure aggregation.")
    .def("dp_eps", &PSContext::dp_eps, "Get dp epsilon for federated learning secure aggregation.")
    .def("set_dp_delta", &PSContext::set_dp_delta, "Set dp delta for federated learning secure aggregation.")
    .def("dp_delta", &PSContext::dp_delta, "Get dp delta for federated learning secure aggregation.")
    .def("set_dp_norm_clip", &PSContext::set_dp_norm_clip,
         "Set dp norm clip for federated learning secure aggregation.")
    .def("dp_norm_clip", &PSContext::dp_norm_clip, "Get dp norm clip for federated learning secure aggregation.")
    .def("set_encrypt_type", &PSContext::set_encrypt_type,
         "Set encrypt type for federated learning secure aggregation.")
    .def("encrypt_type", &PSContext::encrypt_type, "Get encrypt type for federated learning secure aggregation.")
    .def("set_root_first_ca_path", &PSContext::set_root_first_ca_path, "Set root first ca path.")
    .def("root_first_ca_path", &PSContext::root_first_ca_path, "Get root first ca path.")
    .def("set_root_second_ca_path", &PSContext::set_root_second_ca_path, "Set root second ca path.")
    .def("root_second_ca_path", &PSContext::root_second_ca_path, "Get root second ca path.")
    .def("set_pki_verify", &PSContext::set_pki_verify, "Set pki verify.")
    .def("pki_verify", &PSContext::pki_verify, "Get pki verify.")
    .def("set_scheduler_manage_port", &PSContext::set_scheduler_manage_port,
         "Set scheduler manage port used to scale out/in.")
    .def("scheduler_manage_port", &PSContext::scheduler_manage_port, "Get scheduler manage port used to scale out/in.")
    .def("set_equip_crl_path", &PSContext::set_equip_crl_path, "Set root second crl path.")
    .def("set_replay_attack_time_diff", &PSContext::set_replay_attack_time_diff, "Set replay attack time diff.")
    .def("equip_crl_path", &PSContext::equip_crl_path, "Get root second crl path.")
    .def("replay_attack_time_diff", &PSContext::replay_attack_time_diff, "Get replay attack time diff.")
    .def("set_enable_ssl", &PSContext::set_enable_ssl, "Set PS SSL mode enabled or disabled.")
    .def("enable_ssl", &PSContext::enable_ssl, "Get PS SSL mode enabled or disabled.")
    .def("set_client_password", &PSContext::set_client_password, "Set the client password to decode the p12 file.")
    .def("client_password", &PSContext::client_password, "Get the client password to decode the p12 file.")
    .def("set_server_password", &PSContext::set_server_password, "Set the server password to decode the p12 file.")
    .def("server_password", &PSContext::server_password, "Get the server password to decode the p12 file.")
    .def("set_config_file_path", &PSContext::set_config_file_path,
         "Set configuration files required by the communication layer.")
    .def("config_file_path", &PSContext::config_file_path,
         "Get configuration files required by the communication layer.")
    .def("set_dp_eps", &PSContext::set_dp_eps, "Set dp epsilon for federated learning secure aggregation.")
    .def("set_dp_delta", &PSContext::set_dp_delta, "Set dp delta for federated learning secure aggregation.")
    .def("set_dp_norm_clip", &PSContext::set_dp_norm_clip,
         "Set dp norm clip for federated learning secure aggregation.")
    .def("set_encrypt_type", &PSContext::set_encrypt_type,
         "Set encrypt type for federated learning secure aggregation.")
    .def("set_http_url_prefix", &PSContext::set_http_url_prefix, "Set http url prefix for http communication.")
    .def("http_url_prefix", &PSContext::http_url_prefix, "http url prefix for http communication.");

  (void)m.def("_encrypt", &luojianet_ms::pipeline::PyEncrypt, "Encrypt the data.");
  (void)m.def("_decrypt", &luojianet_ms::pipeline::PyDecrypt, "Decrypt the data.");
  (void)m.def("_is_cipher_file", &luojianet_ms::pipeline::PyIsCipherFile, "Determine whether the file is encrypted");

#ifndef _WIN32
  (void)m.def("_export_bprop_mindir", &luojianet_ms::ad::KPrim::ExportBpropMindir,
              "Export the backpropagation function to mindir file.");
#endif
}
