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

#ifndef LUOJIANET_MS_CCSRC_PS_PARAMETER_SERVER_H_
#define LUOJIANET_MS_CCSRC_PS_PARAMETER_SERVER_H_

#include <unistd.h>
#include <string>
#include <iostream>
#include <memory>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <cmath>
#include <random>
#include <utility>
#include <list>
#include <map>
#include <functional>
#include <algorithm>

#include "utils/hash_map.h"
#include "ir/func_graph.h"
#include "backend/session/session_basic.h"
#include "backend/session/anf_runtime_algorithm.h"
#include "backend/session/session_factory.h"
#include "ps/optimizer_info.h"
#include "ps/optimizer_info_builder.h"
#include "ps/ps_context.h"
#include "runtime/device/cpu/kernel_select_cpu.h"
#include "utils/ms_context.h"
#include "backend/kernel_compiler/kernel.h"
#include "backend/kernel_compiler/cpu/cpu_kernel_factory.h"
#include "backend/kernel_compiler/cpu/ps/pserver_kernel.h"
#include "backend/kernel_compiler/cpu/ps/sparse_apply_adam_ps_kernel.h"
#include "backend/kernel_compiler/cpu/ps/sparse_apply_lazy_adam_ps_kernel.h"
#include "backend/kernel_compiler/cpu/ps/sparse_apply_ftrl_ps_kernel.h"
#include "backend/kernel_compiler/cpu/ps/apply_momentum_ps_kernel.h"
#include "backend/kernel_compiler/cpu/ps/embedding_look_up_ps_kernel.h"
#include "ps/ps_cache/ps_data/ps_data_prefetch.h"
#include "ps/random_normal/random_normal.h"
#include "distributed/persistent/data.h"

#include "ps/constants.h"
#include "ps/util.h"
#include "ps/embedding_table_shard_metadata.h"
#include "utils/log_adapter.h"
#include "proto/comm.pb.h"
#include "proto/ps.pb.h"
#include "ps/core/server_node.h"
#include "ps/core/node.h"

namespace luojianet_ms {
namespace ps {
class ParameterServer {
 public:
  static ParameterServer &GetInstance() {
    static ParameterServer instance;
    return instance;
  }

  void Run(const FuncGraphPtr &func_graph);

 private:
  ParameterServer()
      : pserver_num_(0),
        worker_num_(0),
        grad_accum_count_(0),
        handler_(nullptr),
        func_graph_(nullptr),
        sess_(nullptr),
        running_(true),
        thread_(nullptr),
        persist_thread_(nullptr),
        server_node_(nullptr) {}
  ~ParameterServer() = default;
  ParameterServer(const ParameterServer &) = delete;
  ParameterServer &operator=(const ParameterServer &) = delete;

  class ServerHandler {
   public:
    explicit ServerHandler(ParameterServer *ps) : ps_(ps) {}
    ~ServerHandler() = default;
    void Init();
    void operator()(const std::shared_ptr<core::TcpConnection> &conn, const std::shared_ptr<core::MessageMeta> &meta,
                    const DataPtr &data, size_t size);
    void HandlePushReq(const DataPtr &data, size_t size, const VectorPtr &res);
    void HandlePullReq(const DataPtr &data, size_t size, const VectorPtr &res);
    void HandleInitWeights(const DataPtr &data, size_t size, const VectorPtr &res);
    void HandleInitWeightToOptimId(const DataPtr &data, size_t size, const VectorPtr &res);
    void HandleInitInputsShape(const DataPtr &data, size_t size, const VectorPtr &res);
    void HandleInitEmbeddings(const DataPtr &data, size_t size, const VectorPtr &res);
    void HandleCheckReadyForPush(const DataPtr &data, size_t size, const VectorPtr &res);
    void HandleCheckReadyForPull(const DataPtr &data, size_t size, const VectorPtr &res);
    void HandleEmbeddingLookup(const DataPtr &data, size_t size, const VectorPtr &res);
    void HandleUpdateEmbeddings(const DataPtr &data, size_t size, const VectorPtr &res);
    void HandleFinalize(const DataPtr &data, size_t size, const VectorPtr &res);

   private:
    ParameterServer *ps_;
    typedef void (ServerHandler::*RequestHandler)(const DataPtr &data, size_t size, const VectorPtr &res);
    luojianet_ms::HashMap<int, RequestHandler> handlers_;
    luojianet_ms::HashMap<int, std::string> commands_;
    luojianet_ms::HashMap<Key, bool> init_weights_;
    luojianet_ms::HashMap<Key, bool> init_weight_to_optim_;
    luojianet_ms::HashMap<Key, bool> init_optim_info_;
  };

  // For disaster recovery, you can customize the key-value structure that needs to be persisted, and you can customize
  // the business layer disaster recovery function.
  class RecoverHandler {
   public:
    explicit RecoverHandler(ParameterServer *ps) : ps_(ps) {}
    ~RecoverHandler() = default;

    // Initialize storage module and file storage is currently used.
    void Init();

    // Do disaster recovery.
    void Recover();

    core::FileConfiguration *config_storage() const { return storage_.get(); }

   private:
    // Load embedding information from persistent storage to recover embedding table.
    void RecoverEmbedding();

    ParameterServer *ps_;
    typedef void (RecoverHandler::*RecoverFunc)();
    luojianet_ms::HashMap<std::string, RecoverFunc> handlers_;
    std::unique_ptr<core::FileConfiguration> storage_{nullptr};
  };

  bool Init(const FuncGraphPtr &func_graph);
  void InitOptimInfoBuilders();
  void InitWeightKeyToOptims(const Key &key, const int64_t &optim_id);
  void InitOptimInputsShape(const Keys &keys, const Values &values, const Lengths &lengths);
  void InitWeight(const Key &key, const WeightPtr &weight);
  void InitGrad(const Key &key, const GradPtr &grad);
  void InitEmbeddingTable(const Key &key,
                          const std::shared_ptr<std::vector<std::shared_ptr<std::vector<size_t>>>> &shapes,
                          const ParamInitInfo &param_init_info);
  bool HasWeight(const Key &key);
  void Finalize();
  void UpdateWeights();
  void AccumGrad(const Keys &key, const Values &values, const Lengths &lengths);
  WeightPtr weight(const Key &key);
  void DoEmbeddingLookup(Key key, const LookupIds &lookup_ids, KVMessage *res);
  void UpdateEmbeddings(const Key &key, const LookupIds &lookup_ids, const Values &vals);
  inline bool ReadyForUpdateWeights() const;
  inline bool ReadyForPush(const Key &key);
  inline bool ReadyForPull(const Key &key);
  inline void ResetGradAccumCount();
  const CNodePtr GetCNode(const std::string &name) const;
  inline std::mutex &mutex();
  void GetEmbeddingTableParamPtr();
  void SyncEmbeddingTables();
  // Cache embedding table parameter by map, key: parameter name, value: parameter node pointer
  void CacheEmbeddingTableParamPtr();

  // Whether enable disaster recovery.
  bool EnableRecovery() const;

  // Persist weight periodically, trigger by scheduler.
  void PersistParameters();

  // Persist sparse network operators when receive init embedding table message.
  void PersistKernels(const Key &key, const std::shared_ptr<std::vector<std::shared_ptr<std::vector<size_t>>>> &shapes,
                      const ParamInitInfo &param_init_info) const;

  // Persist parameters store in parameter server when receive init message.
  void PersistInitParameters(const Key &key, const WeightPtr &param);

  // Restore sparse network operators and parameters.
  void RecoverEmbedding(const std::vector<Key> &keys, const std::vector<std::vector<std::vector<size_t>>> &shapes_list,
                        const std::vector<std::string> &param_names);

  // Restore sparse network operators.
  void RecoverKernels(const std::vector<Key> &keys, const std::vector<std::vector<std::vector<size_t>>> &shapes_list,
                      const std::vector<std::string> &param_names);

  // Restore parameters store in parameter server.
  void RecoverParameters(const std::vector<Key> &keys);

  // Update the indices of modified part of the persistent parameter.
  void UpdateDirtyInfo(const Key &key, const LookupIds &lookup_ids, int64_t offset);

  // Ser current persistent state to server node.
  void set_persistent_state(core::PersistentState persistent_state) const;

  std::unique_ptr<RecoverHandler> recover_handler_;
  std::atomic_bool finish_recovery_{false};

  size_t pserver_num_;
  size_t worker_num_;
  size_t grad_accum_count_;
  std::unique_ptr<ServerHandler> handler_;
  FuncGraphPtr func_graph_;
  std::shared_ptr<session::SessionBasic> sess_;
  bool running_;
  bool embedding_param_ptr_cached_{false};
  // Used to cache embedding table parameter, key: parameter name, value: parameter node pointer
  luojianet_ms::HashMap<std::string, ParameterPtr> embedding_parameter_tables_;
  // Used to cache the modified part of the parameter.
  luojianet_ms::HashMap<Key, distributed::storage::DirtyInfo> weights_dirty_info_;

  luojianet_ms::HashMap<Key, std::shared_ptr<PServerKernel>> optimizers_;
  luojianet_ms::HashMap<Key, InputsShapePtr> optim_inputs_shape_;
  luojianet_ms::HashMap<Key, InputsShapePtr> original_optim_inputs_shape_;
  luojianet_ms::HashMap<Key, std::shared_ptr<OptimizerInfo>> optim_infos_;
  luojianet_ms::HashMap<std::string, std::shared_ptr<OptimizerInfoBuilder>> optim_info_builders_;
  luojianet_ms::HashMap<Key, std::string> weight_key_to_optims_;
  luojianet_ms::HashMap<Key, std::string> weight_key_to_optim_op_;
  luojianet_ms::HashMap<Key, WeightPtr> weights_;
  luojianet_ms::HashMap<Key, bool> is_embedding_;
  luojianet_ms::HashMap<Key, GradPtr> grads_;
  luojianet_ms::HashMap<Key, size_t> grads_accum_counter_;
  luojianet_ms::HashMap<Key, std::shared_ptr<PServerKernel>> embedding_lookup_ops_;
  luojianet_ms::HashMap<Key, uint64_t> tokens_;

  std::mutex mutex_;
  std::condition_variable apply_grads_cv_;

  std::mutex access_weight_mutex_;
  std::unique_ptr<std::thread> thread_;
  std::unique_ptr<std::thread> persist_thread_;
  std::shared_ptr<core::ServerNode> server_node_;
  std::map<Key, ParameterPtr> embedding_tables_;

  friend class ServerHandler;
};
}  // namespace ps
}  // namespace luojianet_ms
#endif  // LUOJIANET_MS_CCSRC_PS_PARAMETER_SERVER_H_
