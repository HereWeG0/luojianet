#!/usr/bin/env python3
# coding: utf-8
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

import os
import json
from functools import reduce
from akg.global_configs import get_dump_ir_flag


class Graph():
    def __init__(self, output):
        self.tensors = set(output)
        self.ops = []
        self.output_name = output
        self.input_name = []
        self.input = []
        self.core_num = 0
        self.output = []
        self.op_name = 'Fused'

class Liveness():
    def __init__(self):
        self.start = -1
        self.end = -1
        self.is_reduce = False

    def __str__(self):
        return "live_" + str(self.start) + "_" + str(self.end) + "_" + str(self.is_reduce)

    def __repr__(self):
        return "live_" + str(self.start) + "_" + str(self.end) + "_" + str(self.is_reduce)


def _is_reduce(tensor_name):
    return tensor_name.startswith('Reduce')


def _is_tensor(op_info):
    return 'value' not in op_info


def _get_type_bytes(data_type):
    type_bytes_map = {'float32': 4, 'float16': 2}
    return type_bytes_map.get(data_type, 1)


def _liveness_analysis(desc_d, req_map):
    req_liveness = dict((k, Liveness()) for k in req_map.keys())
    idx = len(desc_d['op_desc'])
    for i in range(len(desc_d['op_desc']) - 1, -1, -1):
        idx -= 1
        op_info = desc_d['op_desc'][i]
        for out_desc in op_info['output_desc']:
            out_name = out_desc['tensor_name']
            if out_name in req_liveness:
                if _is_reduce(op_info['name']):
                    req_liveness[out_name].is_reduce = True
                if req_liveness[out_name].end == -1:
                    req_liveness[out_name].end = idx
                    req_liveness[out_name].start = idx
                else:
                    req_liveness[out_name].start = idx
            for input_desc in op_info['input_desc']:
                for sub_input_desc in input_desc:
                    inp_name = sub_input_desc['tensor_name']
                    if inp_name in req_liveness and req_liveness[inp_name].end == -1:
                        req_liveness[inp_name].end = idx
                    if inp_name in req_liveness and req_liveness[inp_name].end > -1:
                        req_liveness[inp_name].start = idx
    # sort req_liveness by Liveness.end.
    sort_req_liveness = dict(sorted(req_liveness.items(), key=lambda x: x[1].end, reverse=True))
    return sort_req_liveness


def _shared_memory_optimization(desc_d, req_map, outputs):
    sort_req_liveness = _liveness_analysis(desc_d, req_map)
    sort_req_buf = list(sort_req_liveness.keys())
    alloc_map = dict()
    reuse_map = dict()
    reverse_reuse_map = dict()
    for i in range(len(sort_req_liveness)):
        reuse = False
        find_conflict = False
        if sort_req_liveness[sort_req_buf[i]].is_reduce:
            alloc_map[sort_req_buf[i]] = [req_map[sort_req_buf[i]][1], req_map[sort_req_buf[i]][0]]
            continue
        for j in range(len(sort_req_liveness) - 1, i, -1):
            if desc_d['process'] == 'aicore':
                continue
            # whether reusable.
            # rule1: one buffer start larger equal to the reused buffer end.
            if sort_req_liveness[sort_req_buf[i]].start >= sort_req_liveness[sort_req_buf[j]].end:
                # rule2: sizes are compatible.
                if req_map[sort_req_buf[i]][0] <= req_map[sort_req_buf[j]][0] and sort_req_buf[j] not in outputs:
                    # rule3: make sure the candidate reused buffer is not using by other conflict variable.
                    for item in reverse_reuse_map.get(sort_req_buf[j], []):
                        if (sort_req_liveness[item].end >= sort_req_liveness[sort_req_buf[i]].end) \
                                or (sort_req_liveness[item].end >= sort_req_liveness[sort_req_buf[i]].start):
                            find_conflict = True
                            break
                    if not find_conflict:
                        if sort_req_buf[j] not in reverse_reuse_map:
                            reverse_reuse_map[sort_req_buf[j]] = [sort_req_buf[i]]
                        else:
                            reverse_reuse_map[sort_req_buf[j]].append(sort_req_buf[i])
                        # rule4: prefer to reuse buffer with same size.
                        if req_map[sort_req_buf[i]][0] == req_map[sort_req_buf[j]][0]:
                            reuse_map[sort_req_buf[i]] = [sort_req_buf[j], req_map[sort_req_buf[i]][0]]
                            reuse = True
                            break
                        else:
                            reuse_map[sort_req_buf[i]] = [sort_req_buf[j], req_map[sort_req_buf[i]][0]]
                            reuse = True
        if not reuse:
            alloc_map[sort_req_buf[i]] = [req_map[sort_req_buf[i]][1], req_map[sort_req_buf[i]][0]]
    return alloc_map, reuse_map


def _parse_merged_json(desc_d, stitch_tensor_name, input_tensor_name, output_tensor_name):
    """
    Parse merged json to get subgraph splitted by stitch nodes and input-output relationship of merged graph.

    Args:
        desc_d (dict): The dict of compute description.
        stitch_tensor_name (list[string]): The list of stitch node tensors.
            stitch nodes are regarded as edges of sub_graphs. The smallest number of sub_graph is the length of
            stitch_tensor_name + 1.

        input_tensor_name (list[string]): The list of input tensors.
        output_tensor_name (list[string]): The list of output tensors.
            output tensors would be regarded as inter_output_tensor and final_output_tensor. The main difference
            of the two kinds of tensors is whether out-degree is zero, in which final_output_tensor is the tensor
            with zero out-degree in merged graph and otherwise, it is inter_output_tensor.

    Returns:

        extra_subgraph_output (dict): The dict of extra output tensors for each sub_graph.
        final_output_list (list[string]): The list of final output tensors.
            output tensors in this list are are final_output_tensor and the subgraph they belong to doesn't
            include stitch nodes.
        final_output_within_graph (list[string]): The list of final output tensors.
            output tensors in this list are final_output_tensor and the subgraph they belong to also includes
            stitch node.

    """
    # Initialize sub_graph number as the smallest possible number of sub graph.
    # sub graphs number might increase based on graph structure.
    sub_graph_length = len(stitch_tensor_name)
    sub_graph_node = [set() for _ in range(sub_graph_length)]
    # use dict to save extra outputs for each sub_graph.
    extra_subgraph_output = dict(zip(stitch_tensor_name, [[] for _ in range(sub_graph_length)]))
    in_out_dict = {}
    inter_output_list = set()
    final_output_list = set()
    final_output_within_graph = []
    idx = 0
    final_output_graph = False
    for i in range(len(desc_d['op_desc']) - 1, -1, -1):
        op_info = desc_d['op_desc'][i]
        for out_desc in op_info['output_desc']:
            # switch to next subgraph if find stitch node.
            if out_desc['tensor_name'] in stitch_tensor_name:
                idx += 1
                cur_stitch_node = out_desc['tensor_name']
                # when current subgraph concludes final output and encounters with stitch node,
                # increase number of subgraph.
                if final_output_graph:
                    final_output_list.add(cur_final_node)
                    final_output_within_graph.remove(cur_final_node)
                    sub_graph_length += 1
                    sub_graph_node += [set()]
                    final_output_graph = False

            # out_desc not in in_out_dict means out-degree is zero.
            if out_desc['tensor_name'] not in in_out_dict:
                final_output_graph = True
                cur_final_node = out_desc['tensor_name']
                final_output_within_graph.append(cur_final_node)

            sub_graph_node[idx].add(out_desc['tensor_name'])
            for input_desc in op_info['input_desc']:
                for sub_input_desc in input_desc:
                    sub_graph_node[idx].add(sub_input_desc['tensor_name'])
                    tmp_name = sub_input_desc['tensor_name']
                    if tmp_name in output_tensor_name:
                        inter_output_list.add(sub_input_desc['tensor_name'])
                    for subgraph in sub_graph_node[0: idx]:
                        extra_output = _is_tensor(sub_input_desc) and tmp_name not in stitch_tensor_name \
                            and tmp_name not in input_tensor_name
                        used_by_other_sg = tmp_name in subgraph
                        used_as_output = tmp_name in output_tensor_name
                        extra_output = extra_output and (used_by_other_sg or used_as_output)
                        if extra_output and cur_stitch_node and not final_output_graph:
                            extra_subgraph_output[cur_stitch_node].insert(0, tmp_name)
                            break
                    if sub_input_desc['tensor_name'] not in in_out_dict:
                        in_out_dict[sub_input_desc['tensor_name']] = [out_desc['tensor_name']]
                    else:
                        in_out_dict[sub_input_desc['tensor_name']].append(out_desc['tensor_name'])

    return extra_subgraph_output, list(final_output_list), final_output_within_graph


def _collect_subgraph_info(desc_d, sub_stitch_graphs, req_map, input_tensor_name, output_tensor_name, stitch_node_list):
    inplace_assign_map = {}
    fake_output_list = []
    # traversal desc_d by reverse topologically order.
    for i in range(len(desc_d['op_desc']) - 1, -1, -1):
        op_info = desc_d['op_desc'][i]
        if op_info['name'] == "InplaceAssign":
            inplace_assign_map[op_info['output_desc'][0]['tensor_name']] = op_info['input_desc'][0][0]['tensor_name']
            if op_info['attr'][0]['name'] == 'fake_output' and op_info['attr'][0]['value'] == 1:
                fake_output_list.append(op_info['output_desc'][0]['tensor_name'])
        for sg in sub_stitch_graphs:
            added_output = []
            for out_desc in op_info['output_desc']:
                out_tensor_name = out_desc['tensor_name']
                if out_tensor_name in sg.tensors:
                    sg.ops.append(op_info)
                    if out_tensor_name in req_map:
                        if out_desc['shape'] and out_desc['data_type']:
                            type_bytes = _get_type_bytes(out_desc['data_type'])
                            req_map[out_tensor_name] = \
                                [reduce(lambda x, y: x * y, out_desc['shape']) * type_bytes, out_desc['data_type']]
                        else:
                            req_map[out_tensor_name] = [1, out_desc['data_type']]

                    if out_tensor_name in sg.output_name and out_tensor_name not in added_output:
                        sg.output.append(out_desc)
                        added_output.append(out_tensor_name)

                    for input_desc in op_info['input_desc']:
                        for sub_input_desc in input_desc:
                            if _is_tensor(sub_input_desc):
                                input_name = sub_input_desc['tensor_name']
                                if input_name in output_tensor_name and input_name not in added_output:
                                    sg.output.insert(0, sub_input_desc)
                                    added_output.append(input_name)
                                if input_name in input_tensor_name and input_name not in sg.input_name:
                                    sg.input_name.append(sub_input_desc['tensor_name'])
                                    sg.input.append([sub_input_desc])
                                # stop expand subgraph when encounter with stitch node.
                                if input_name not in stitch_node_list:
                                    sg.tensors.add(sub_input_desc['tensor_name'])
                                # add extra input into subgraph.
                                elif input_name not in sg.output_name and input_name not in sg.input_name:
                                    sg.input_name.append(input_name)
                                    sg.input.append([sub_input_desc])
    return sub_stitch_graphs, inplace_assign_map, fake_output_list


def _sub_graph_info(sub_graph, desc_d):
    # gather info for sub graph.
    op_json_str = {}
    op_json_str['composite'] = True
    op_json_str['composite_graph'] = desc_d['composite_graph']
    op_json_str['id'] = desc_d['id']
    op_json_str['op'] = sub_graph.op_name
    op_json_str['input_desc'] = sub_graph.input
    op_json_str['op_desc'] = sub_graph.ops
    op_json_str['output_desc'] = sub_graph.output
    op_json_str['platform'] = "AKG"
    op_json_str['process'] = desc_d['process']
    if 'sub_block_size' in desc_d['buffer_stitch']:
        op_json_str['blocksize'] = desc_d['buffer_stitch']['sub_block_size']

    json_str = json.dumps(op_json_str)
    return json_str


def stitch_json_split(desc_d):
    """
    split sub graph from merged json file.
    Using 'buffer_stitch' to store stitch info from graph kernel.
    Args:
        desc_d: dict of compute description
    Returns:
        List of spilted json info.
        List of original input.
        Dict of dominance info.
    """
    stitch_jsons = []

    input_tensor_name = [tensor[0]['tensor_name'] for tensor in desc_d['input_desc']]
    output_tensor_name = [tensor['tensor_name'] for tensor in desc_d['output_desc']]
    stitch_node = desc_d['buffer_stitch']['stitch_op']
    stitch_node_name = [node for stitchnode in stitch_node for node in stitchnode]
    extra_subgraph_output, final_output_list, final_output_within_graph = \
        _parse_merged_json(desc_d, stitch_node_name, input_tensor_name, output_tensor_name)

    # traverse extra_subgraph_output to save extra output into subgraph.
    stitch_node = []
    extra_list = []
    for item in extra_subgraph_output:
        cur_list = [item]
        for node in extra_subgraph_output[item]:
            if node not in extra_list:
                extra_list.append(node)
                cur_list.append(node)
        stitch_node.append(cur_list)
    stitch_node_name = [node for stitchnode in stitch_node for node in stitchnode]

    # initialize req_map
    req_op_size = [0] * len(stitch_node_name)
    req_map = dict(zip(stitch_node_name, req_op_size))
    # add final output within subgraph into the last initialized stitch sub_graph.
    stitch_node = stitch_node[:-1] + [stitch_node[-1] + final_output_within_graph]
    # add final output into stitch_op.
    stitch_node += [[op] for op in final_output_list if op not in stitch_node_name]
    stitchnode_list = [node for stitchnode in stitch_node for node in stitchnode]
    # each output tensor can only be parsed as output once in all subgraphs.
    # All tensors in stitch_node_list will be put into output_name.
    # Save other output tensors which are not in stitch_node_name for the output collection of subgraphs.
    complement_output = [tensor for tensor in output_tensor_name if tensor not in stitchnode_list]

    # initialize sub_stitch_graphs.
    sub_stitch_graphs = []
    for i, stitch_op in enumerate(stitch_node):
        sub_stitch_graphs.append(Graph(stitch_op))

    sub_stitch_graphs, inplace_assign_map, fake_output_list = \
        _collect_subgraph_info(desc_d, sub_stitch_graphs, req_map,
                               input_tensor_name, complement_output, stitchnode_list)
    # reverse op order to generate topological subgraph
    for i, sg in enumerate(sub_stitch_graphs):
        sg.ops = list(reversed(sg.ops))
        sg.op_name = desc_d['op']
        stitch_json_str = _sub_graph_info(sg, desc_d)
        if os.getenv(get_dump_ir_flag()) == "on":
            if not os.path.exists("stitch_info"):
                try:
                    os.mkdir("stitch_info")
                except OSError as err:
                    # 17, OSError: [Errno 17] File exists
                    if err.errno == 17:
                        pass
                    else:
                        raise err
            with open('stitch_info/' + sg.op_name + '_stitch_' + str(i + 1) + '.json', 'w+') as f:
                f.write(stitch_json_str)
            with open('stitch_info/' + sg.op_name + '_stitch.json', 'w+') as f:
                f.write(json.dumps(desc_d))
        stitch_jsons.append(stitch_json_str)

    clean_op_list = [fake_op for fake_op in fake_output_list if fake_op in stitch_node_name]
    # add fake outputs into output_tensor_name
    output_tensor_name += clean_op_list
    # start node for dominance tree is final_output_list + final_output_within_graph.
    start_node = final_output_list + final_output_within_graph
    alloc_map, reuse_map = _shared_memory_optimization(desc_d, req_map, output_tensor_name)
    # remove fake output from alloc_map and store them into clean_op_map
    clean_op_map = dict()
    for fake_op in clean_op_list:
        if fake_op in alloc_map:
            clean_info = alloc_map[fake_op]
            alloc_map.pop(fake_op)
        else:
            clean_info = reuse_map[fake_op]
            reuse_map.pop(fake_op)
        clean_op_map[inplace_assign_map[fake_op]] = clean_info

    if not alloc_map:
        alloc_map['EMPTY'] = []
    if not clean_op_map:
        clean_op_map['EMPTY'] = []
    if not reuse_map:
        reuse_map['EMPTY'] = []
    return stitch_jsons, input_tensor_name, output_tensor_name, alloc_map, reuse_map, clean_op_map


def split_stitch_attr(attr, split_num):
    common_attr = {}
    sub_attr = [{} for _ in range(split_num)]
    if attr is None or not isinstance(attr, dict):
        return common_attr, sub_attr
    for k, v in attr.items():
        if isinstance(k, str) and k.startswith("sub_attr_"):
            continue
        common_attr[k] = v
    for i in range(split_num):
        key = "sub_attr_" + str(i + 1)
        if key in attr:
            for k, v in attr[key].items():
                sub_attr[i][k] = v
    return common_attr, sub_attr


def combine_stitch_attr(common_attr, sub_attr):
    attr = {}
    for k, v in common_attr.items():
        attr[k] = v
    for i, a in enumerate(sub_attr):
        if not a:
            continue
        key = "sub_attr_" + str(i + 1)
        attr[key] = {}
        for k, v in a.items():
            attr[key][k] = v
    return attr
