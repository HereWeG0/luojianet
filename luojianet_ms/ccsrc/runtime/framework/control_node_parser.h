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

#ifndef LUOJIANET_MS_CCSRC_RUNTIME_FRAMEWORK_CONTROL_NODE_PARSER_H_
#define LUOJIANET_MS_CCSRC_RUNTIME_FRAMEWORK_CONTROL_NODE_PARSER_H_

#include <vector>
#include <string>
#include <memory>
#include <set>
#include <queue>
#include <map>
#include <stack>
#include <utility>
#include <unordered_map>
#include <algorithm>
#include "utils/hash_map.h"
#include "runtime/hardware/device_context.h"
#include "backend/session/kernel_graph.h"

namespace luojianet_ms {
namespace runtime {
using luojianet_ms::device::DeviceContext;
using luojianet_ms::session::KernelGraph;
using luojianet_ms::session::KernelWithIndex;

constexpr int kInvalidBranchID = -1;
constexpr int kMainBranchID = 0;
constexpr int kSubBranchStartID = 1;
constexpr size_t kSwitchInputNum = 4;
constexpr size_t kSwitchCondPos = 1;
constexpr size_t kSwitchPartialNum = 2;
constexpr size_t kSwitchLayerCondPos = 1;
constexpr size_t kSwitchLayerBranchPos = 2;
constexpr size_t kSwitchLayerInputNum = 3;
constexpr size_t kSwitchTrueBranchPos = 2;
constexpr size_t kSwitchFalseBranchPos = 3;
constexpr size_t kPartialFuncGraphPos = 1;
constexpr size_t kPartialInputStartPos = 2;
constexpr size_t kCallInputStartPos = 1;
constexpr size_t kMakeTupleInputStartPos = 1;
constexpr size_t kCNodeInputStartPos = 1;
constexpr size_t kReturnInputPos = 1;
constexpr size_t kSingleControlNode = 1;

constexpr size_t kCsrTensorIndPtrIndex = 0;
constexpr size_t kCsrTensorIndicesIndex = 1;
constexpr size_t kCsrTensorValuesIndex = 2;
constexpr size_t kCsrTensorDenseShapeIndex = 3;
constexpr size_t kMakeCSRTensorInputStartPos = 1;
constexpr size_t kMakeCSRTensorInputNum = 4;

const char kEntranceActorNameSuffix[] = "_EntranceActor";
const char kExitActorNameSuffix[] = "_ExitActor";
const char kStackActorNameSuffix[] = "_StackActor";

using FrontToBackendNodeWithContext = std::map<KernelWithIndex, std::set<std::pair<AnfNodePtr, DeviceContext *>>>;
using FrontToBackendKernelWithContext = std::map<KernelWithIndex, std::pair<KernelWithIndex, DeviceContext *>>;
using FuncGraphToKernelGraphGroup = luojianet_ms::HashMap<FuncGraphPtr, std::vector<std::vector<KernelGraphPtr>>>;
using HostParameterToWeight = std::map<AnfNodePtr, std::set<AnfNodePtr>>;
using NodeWithDeviceContext = std::set<std::pair<KernelWithIndex, const DeviceContext *>>;
using RealToFormalNode = luojianet_ms::HashMap<AnfNodePtr, std::vector<AnfNodePtr>>;
using FormalToRealParameter = std::map<KernelWithIndex, std::set<KernelWithIndex>>;
using RealToFormalParameter = std::map<KernelWithIndex, std::set<KernelWithIndex>>;
using KernelBuildInfoBuilder = kernel::KernelBuildInfo::KernelBuildInfoBuilder;
using FrontNodeToKernelGraph = luojianet_ms::HashMap<AnfNodePtr, KernelGraphPtr>;
using FuncGraphCallRelation = luojianet_ms::HashMap<FuncGraphPtr, std::vector<std::set<FuncGraphPtr>>>;
using CallNodeToFuncGraph = luojianet_ms::HashMap<AnfNodePtr, std::set<FuncGraphPtr>>;
using KernelGraphToDeviceContext = luojianet_ms::HashMap<KernelGraphPtr, DeviceContext *>;
// In the control flow, heterogeneous kernel graphs need to be reconnected in the same group, and the kernel graph
// group info is used to store the inputs and outputs of the group.
struct KernelGraphGroupInfo {
  bool is_call_input_;
  std::string group_name_;
  std::set<KernelGraphPtr> graphs_;
  std::map<KernelWithIndex, const DeviceContext *> front_input_nodes_;
  FrontToBackendKernelWithContext front_output_nodes_;
};
using KernelGraphGroupInfoPtr = std::shared_ptr<KernelGraphGroupInfo>;

// Check whether the parameter is a weight. In the control flow, weight is passed to the subgraph, and in the subgraph,
// it is determined whether it is a weight.
bool HasAbstractRef(const AnfNodePtr &node);
// Check whether the node is a csr node.
bool IsCsrNode(const AnfNodePtr &node);
// Get the front node corresponding to the backend node, if the front node is not a parameter node, return the
// corresponding cnode.
KernelWithIndex GetFrontNodeByKernelGraph(const AnfNodePtr &backend_node, KernelGraph *const graph);
// Get all the real input of the frontend node, skip the virtual node like maketuple, tuplegetitem.
std::vector<KernelWithIndex> FetchInputNodeByCNode(const AnfNodePtr &node);
// Fetch the sub abstract from the top abstract by the index.
abstract::AbstractBasePtr FetchAbstractByIndex(const AbstractBasePtr &abstract, size_t index);
// ControlNodeParser is used to parse control nodes, and get the edges between nodes.
class ControlNodeParser {
 public:
  // Parse the control node and put the results of the parsing into member variables.
  void Parse(const std::vector<AnfNodePtr> &control_nodes, const std::vector<KernelGraphPtr> &graphs,
             const std::vector<DeviceContext *> &device_contexts, const FuncGraphPtr &root_graph,
             const FuncGraphToKernelGraphGroup &func_graph_to_kernel_graphs);

  bool IsInited() { return is_inited_; }
  // Check whether there is a call node in the front input nodes of the kernel graph.
  bool IsCallInputKernelGraph(KernelGraph *const graph);
  // Check whether there is a call node in the front input nodes of the kernel graph group.
  bool IsCallInputKernelGraphGroup(const std::string &group_name);
  // Check whether the data arrow of the kernel actor needs to be connected to the control actor.
  // There are two situations:
  // 1. In control flow, the parameter input needs to be connected to the entrance actor of the funcgraph.
  // 2. In the kernel graph with call node input, the data arrow needs to be connected to the stack actor.
  bool IsControlFlowDataArrow(const KernelGraphPtr &graph, const AnfNodePtr &node);
  bool IsRootGraphParameter(const AnfNodePtr &node);
  bool IsRecursionCallNode(const AnfNodePtr &node);
  // If there is a recursive call node in the input of the kernel graph, the graph is recursive.
  bool IsRecursionKernelGraph(const KernelGraphPtr &graph);
  bool IsSameKernelGraphGroup(const AnfNodePtr &node, const KernelGraphPtr &graph);

  const std::vector<AnfNodePtr> &control_node_parameters() const { return control_node_parameters_; }
  const FrontToBackendNodeWithContext &front_to_backend_parameters() const { return front_to_backend_parameters_; }
  const HostParameterToWeight &host_parameter_to_weights() const { return host_parameter_to_weights_; }
  const NodeWithDeviceContext &front_value_nodes() const { return front_value_nodes_; }

  // Fetch all funcgraphs that the call node may call.
  const std::set<FuncGraphPtr> &FetchFuncGraphbyCallNode(const AnfNodePtr &control_node);
  // Fetch the branch id corresponding to funcgraph.
  int FetchBranchIDByCallNode(const AnfNodePtr &call_node);
  // Fetch the kernel graph which the kernel belongs.
  KernelGraphPtr FetchKernelGraphByFrontNode(const AnfNodePtr &kernel);
  // Fetch the backend kernel of front node.
  KernelWithIndex FetchBackendNodeByFrontNode(const KernelWithIndex &node_with_index);
  FuncGraphPtr FetchFuncGraphByKernelGraph(const KernelGraph *const graph);
  std::string FetchGroupNameByKernelGraph(const KernelGraphPtr &graph);

 private:
  friend class GraphScheduler;
  friend class ControlNodeScheduler;
  // Collect all front value nodes. In the control flow, when the input of the switch actor is the value node, these
  // value nodes will not enter the kernel graph, so these nodes need to be saved separately, and space is allocated for
  // them separately during initialization.
  // The interface is initialized by finding the backend node in the kernel graph that the front node finally sends to.
  void FetchFrontValueNode(const std::vector<AnfNodePtr> &control_nodes, DeviceContext *default_context);
  // Create branch id for all call node in the control flow.
  void CreateBranchIDForCallNode(const std::vector<AnfNodePtr> &control_nodes);

  // Parse all the relationships between front parameters and backend parameters.The front parameters
  // include two parts:
  // 1. The parameter from kernel graph.
  // 2. The parameter from control nodes.
  void ParseFrontToBackendParameter(const std::vector<KernelGraphPtr> &graphs,
                                    const std::vector<DeviceContext *> &device_contexts);
  // The relationship between front parameters indicates that the parameter is directly used as the input of the
  // funcgraph. There are two situations:
  // 1. The parameter is used as the input of the call node,
  // 2. The parameter is used as the input of the partial and will be input to the funcgraph of the partial in the
  //    subsequent call node.
  void ParseFormalToRealParameter(const std::vector<AnfNodePtr> &control_nodes);
  // Recursively get all the real parameters corresponding to the formal parameters.
  void ParseAllRealParameterByFormalParameter(const KernelWithIndex &formal_parameter,
                                              const FormalToRealParameter &formal_to_real_parameters,
                                              std::set<KernelWithIndex> *total_real_parameters,
                                              std::set<KernelWithIndex> *invalid_real_parameter);
  // Get all the call nodes without a recursion call relation.
  void ParseUnRecursionCallNode();

  // Parse the device context of the control node. In a heterogeneous scenario, different device contexts need to be
  // copied between different device memories. The analysis steps:
  // 1. Get the device context of the funcgraph parameter according to the device type of the kernel in the funcgraph.
  // 2. Determine the type of device context output by funcgraph according to the call relationship of funcgrpah.
  // 3. Determine the type of device context output for the real parameters on the partial nodes and call nodes.
  void ParseDeviceContext(const std::vector<AnfNodePtr> &control_nodes,
                          const std::vector<KernelGraphPtr> &kernel_graphs,
                          const std::vector<DeviceContext *> &device_contexts,
                          const FuncGraphToKernelGraphGroup &func_graph_to_kernel_graphs);
  void ParseDeviceContextForFuncGraph(const std::vector<AnfNodePtr> &control_nodes,
                                      const std::vector<KernelGraphPtr> &kernel_graphs,
                                      const std::vector<DeviceContext *> &device_contexts,
                                      const FuncGraphToKernelGraphGroup &func_graph_to_kernel_graphs);
  void ParseDeviceContextForReturnNode(const DeviceContext *default_context);
  void ParseDeviceContextForCallNode(const std::vector<AnfNodePtr> &control_nodes);
  void ParseDeviceContextForPartialNode(const std::vector<AnfNodePtr> &control_nodes);

  // In the actor model, when the funcgraph comes to an end temporarily, the exit of the funcgraph needs to notify
  // the entrance actor so that it can process next parameters. This is used to obtain the nodes corresponding to all
  // actors in the funcgraph that need to send control messages to the entrance.
  // These node are control nodes without control node input in the topological sort of the funcgraph.
  void ParseFirstControlNodeForFuncGraph(const std::vector<AnfNodePtr> &control_nodes);
  // Parse all funcgraphs that call nodes may call.
  void ParseCallNodeToFuncGraph(const std::vector<AnfNodePtr> &control_nodes);

  // Get the relationship between the front and backend of the executable kernel in all kernel graphs.
  void FetchFrontToBackendKernel(const std::vector<KernelGraphPtr> &graphs,
                                 const std::vector<DeviceContext *> &device_contexts);
  void ParseFrontNodeToKernelGraph(const std::vector<KernelGraphPtr> &graphs);
  // nodes and call nodes of the root funcgraph.
  void ParseControlNodeParameter(const std::vector<AnfNodePtr> &control_nodes);
  // Get all the front weight parameters related to the weight in the host parameter.
  void FetchHostParameterToWeight();
  // Get the dependency between kernel and call node in auto monad.
  void FetchAutoMonadNode(const std::vector<AnfNodePtr> &control_nodes);
  // Fetch the formal parameter in root graph by parameters in subgraph.
  AnfNodePtr FetchRootGraphFrontNodeBySubFrontNode(const AnfNodePtr &sub_front_node);
  // Get the control nodes and kernel graphs which need to add a stack actor for them.
  // When a control node or kernel graph has input that is a call node, you need to add a stack actor for it.
  void ParseNeedStackControlNode(const std::vector<AnfNodePtr> &control_nodes);
  void ParseNeedStackKernelGraph(const KernelGraphToDeviceContext &kernel_graph_to_device_contexts);
  // When the parameter is directly used as the condition of the switch, there will be no back-end node, and a device
  // tensor needs to be created for it.
  void CreateDeviceTensorForRootGraphParameter(DeviceContext *default_context);
  // In control flow, funcgraph will be cut into multiple kernel graphs for execution, and this relationship is recorded
  // in this map.
  FuncGraphToKernelGraphGroup func_graph_to_kernel_graph_groups_;
  // The kernel graph to which the front node belongs after the funcgraph is cut.
  FrontNodeToKernelGraph front_node_to_kernel_graph_;

  // The front to backend parameters is used to build and link the host data source actor in the control flow scenario.
  FrontToBackendNodeWithContext front_to_backend_parameters_;
  // Relationship between the front and backend of the executable kernel in all kernel graphs.
  FrontToBackendKernelWithContext front_to_backend_kernels_;

  // Relationship between formal parameters and real parameters.
  FormalToRealParameter formal_to_real_parameters_;
  RealToFormalParameter real_to_formal_parameters_;

  // Branch id of funcgraph.
  // In control flow, funcgraph will be called in multiple places, and the output of funcgraph needs to return to
  // different places. Therefore, a branch id is created for each funcgraph. When funcgraph is called, the branch
  // id needs to be sent to the gather actor corresponding to the funcgraph, and the gather will send the branch id
  // to its output switch actor.
  luojianet_ms::HashMap<AnfNodePtr, int> call_node_to_branch_id_;
  CallNodeToFuncGraph call_node_to_func_graphs_;
  // host parameter to weights records the weights in the subgraph corresponding to the node in the root funcgraph.
  // When initializing the weights, all related weights need to be recorded as the same device tensor.
  HostParameterToWeight host_parameter_to_weights_;
  luojianet_ms::HashMap<AnfNodePtr, AnfNodePtr> sub_front_node_to_root_front_node_;
  // The front value node saves all value nodes that are not in the kernel graph. These nodes are generally the
  // input of the control node.
  NodeWithDeviceContext front_value_nodes_;

  // Parameters of control node which come from the host actor.
  std::vector<AnfNodePtr> control_node_parameters_;
  // The kernel graph of call exists in the front input node.
  // In the scene of funcgrarph recursive call, general input and call input are passed recursively, so a stack actor
  // is created for kernel graph which has a call input.
  std::set<KernelGraph *> call_input_kernel_graphs_;
  std::set<KernelGraphGroupInfoPtr> kernel_graph_group_infos_;
  // The dependency between kernel and call node in auto monad.
  luojianet_ms::HashMap<AnfNodePtr, AnfNodePtr> kernel_to_call_nodes_;
  // Control nodes without a control node input in the topological sorting of funcgraph.
  luojianet_ms::HashMap<FuncGraphPtr, std::set<AnfNodePtr>> func_graph_to_first_control_nodes_;
  // Call nodes without recursive call. The funcgraphs of the call will not call the funcgraph where the call node
  // belong.
  std::set<AnfNodePtr> unrecursion_call_nodes_;
  // Those control nodes that need to create the corresponding stack actor, when there is a call node in the inputs
  // of the control node, the stack actor is needed to collect these inputs.
  std::set<AnfNodePtr> need_stack_control_nodes_;

  // In heterogeneous scenario, each parameter has its own device context type, so the device context corresponding
  // to the type needs to be parsed in advance so that it can add some copy operation in the scheduler.
  // 1. The device context type of the formal parameters of funcgraph.
  luojianet_ms::HashMap<FuncGraphPtr, std::vector<const DeviceContext *>> func_graph_to_device_contexts_;
  // 2. The device context type of the control node inputs.
  luojianet_ms::HashMap<AnfNodePtr, std::vector<const DeviceContext *>> control_node_to_device_contexts_;

  // Kernel graph to the group info it belongs.
  luojianet_ms::HashMap<KernelGraphPtr, KernelGraphGroupInfoPtr> kernel_graphs_to_group_info_;

  // Is control flow enable.
  bool is_inited_{false};

  // Root funcgraph and its parameters.
  FuncGraphPtr root_func_graph_;
  std::vector<AnfNodePtr> root_graph_parameters_;
};

using ControlNodeParserPtr = std::shared_ptr<ControlNodeParser>;
}  // namespace runtime
}  // namespace luojianet_ms

#endif  // LUOJIANET_MS_CCSRC_RUNTIME_FRAMEWORK_CONTROL_NODE_PARSER_H_
