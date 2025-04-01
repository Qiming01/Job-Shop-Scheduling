//
// Created by qiming on 25-3-31.
//
#include "graph.h"
#include "operation.h"
#include "instance.h"
#include <unordered_set>
#include <algorithm>
#include <unordered_map>
//Graph generate_initial_solution(const Instance& instance) {
//    OperationList op_list(instance);
//    Graph graph;
//
//    graph.job_num = instance.job_num;
//    graph.operation_num = instance.operation_num * instance.job_num; // 实际工序数，假设每个工件有operation_num个工序
//    int total_ops = op_list.operations.size(); // 包含头尾节点
//
//    // 初始化所有边为-1（假设-1表示无连接）
//    graph.adj_job_edges.resize(total_ops, -1);
//    graph.reverse_adj_job_edges.resize(total_ops, -1);
//    graph.adj_machine_edges.resize(total_ops, -1);
//    graph.reverse_adj_machine_edges.resize(total_ops, -1);
//
//    // 处理工件边
//    for (int job_id = 0; job_id < instance.job_num; ++job_id) {
//        for (int cnt = 1; cnt <= instance.operation_num; ++cnt) {
//            int op_id = (job_id) * instance.operation_num + cnt;
//            Operation& current_op = op_list.get_operation_ref(op_id);
//
//            if (cnt < instance.operation_num) {
//                int next_op_id = op_id + 1;
//                graph.adj_job_edges[op_id] = next_op_id;
//                graph.reverse_adj_job_edges[next_op_id] = op_id;
//            } else {
//                // 最后一个工序指向尾节点
//                int tail_id = total_ops - 1;
//                graph.adj_job_edges[op_id] = tail_id;
//                graph.reverse_adj_job_edges[tail_id] = op_id;
//            }
//        }
//        // 设置第一个工序的reverse_job边指向头节点
//        int first_op_id = job_id * instance.operation_num + 1;
//        graph.reverse_adj_job_edges[first_op_id] = 0; // 头节点ID为0
//    }
//
//    // 收集所有机器ID
//    std::unordered_set<int> machine_ids;
//    for (int op_id = 1; op_id < total_ops - 1; ++op_id) {
//        machine_ids.insert(op_list.get_operation_ref(op_id).machine_id);
//    }
//    std::vector<int> machines(machine_ids.begin(), machine_ids.end());
//    std::sort(machines.begin(), machines.end());
//
//    // 处理机器边
//    std::unordered_map<int, std::vector<int>> machine_ops;
//    for (int op_id = 1; op_id < total_ops - 1; ++op_id) {
//        Operation& op = op_list.get_operation_ref(op_id);
//        machine_ops[op.machine_id].push_back(op_id);
//    }
//
//    for (auto& pair : machine_ops) {
//        int m_id = pair.first;
//        std::vector<int>& ops = pair.second;
//
//        // 排序：按job_id和operation_cnt
//        std::sort(ops.begin(), ops.end(), [&op_list, &instance](int a, int b) {
//            Operation& op_a = op_list.get_operation_ref(a);
//            Operation& op_b = op_list.get_operation_ref(b);
//
//            int job_a = (a - 1) / instance.operation_num;
//            int job_b = (b - 1) / instance.operation_num;
//
//            if (job_a != job_b) return job_a < job_b;
//            return op_a.operation_cnt < op_b.operation_cnt;
//        });
//
//        // 构建机器边
//        for (size_t i = 0; i < ops.size(); ++i) {
//            int current = ops[i];
//            if (i < ops.size() - 1) {
//                int next = ops[i + 1];
//                graph.adj_machine_edges[current] = next;
//                graph.reverse_adj_machine_edges[next] = current;
//            }
//            if (i > 0) {
//                int prev = ops[i - 1];
//                graph.reverse_adj_machine_edges[current] = prev;
//            }
//        }
//
//        graph.first_op_in_machine.push_back(ops.front());
//        graph.last_op_in_machine.push_back(ops.back());
//    }
//
//    return graph;
//}