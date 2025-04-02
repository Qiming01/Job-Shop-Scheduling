//
// Created by qiming on 25-3-31.
//
#include "graph.h"
#include "operation.h"
#include "instance.h"
#include <unordered_set>
#include <algorithm>
#include <unordered_map>
#include <random>
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

/**
 * 使用Giffler-Thompson算法生成JSSP问题的随机初始解
 * @param instance JSSP问题实例
 * @return 生成的调度图
 */
Graph generateRandomInitialSolution(const Instance &instance) {
    int job_num = instance.job_num;
    int operation_num = instance.operation_num;

    // 初始化调度图
    Graph graph;
    graph.job_num = job_num;
    graph.operation_num = operation_num;

    // 初始化边
    int total_operations = job_num * operation_num + 2; // 包含虚拟头尾节点
    graph.operation_edges.resize(total_operations, -1);
    graph.machine_edges.resize(total_operations, -1);
    graph.reverse_operation_edges.resize(total_operations, -1);
    graph.reverse_machine_edges.resize(total_operations, -1);

    // 初始化工件的第一个和最后一个工序
    graph.first_job_operation.resize(job_num, -1);
    graph.last_job_operation.resize(job_num, -1);
    graph.first_machine_operation.resize(operation_num, -1);
    graph.last_machine_operation.resize(operation_num, -1);

    // 将工序按照工件顺序组织
    OperationList operations(instance);

    // 创建机器上的工序列表
    MachineOperation machine_operation(instance);

    // Giffler-Thompson算法实现
    // 1. 初始化
    std::vector<bool> scheduled(job_num * operation_num + 1, false); // 标记哪些工序已经被调度
    std::vector<int> machine_ready_time(operation_num, 0); // 每台机器的就绪时间
    std::vector<int> job_ready_time(job_num, 0); // 每个工件的就绪时间

    // 工件的当前工序索引
    std::vector<int> current_job_operation(job_num, 0);

    // 2. 主循环：直到所有工序都被调度
    for (int scheduled_count = 0; scheduled_count < job_num * operation_num; scheduled_count++) {
        // 找出所有可以调度的工序
        std::vector<std::pair<int, int>> candidates; // (job_id, op_index)

        // 遍历所有的工件
        for (int job_id = 0; job_id < job_num; job_id++) {
            int op_index = current_job_operation[job_id];   // 当前工件执行到哪一个工序了
            if (op_index < operation_num) { // 还有工序未被调度
                candidates.emplace_back(job_id, op_index);
            }
        }

        // 找出最早完成的工序
        int earliest_completion_time = INT_MAX;
        std::vector<std::pair<int, int>> earliest_ops;

        for (auto& candidate : candidates) {
            int job_id = candidate.first;
            int op_index = candidate.second;

            int machine_id = instance.data[job_id][op_index].first;
            int process_time = instance.data[job_id][op_index].second;

            int start_time = std::max(machine_ready_time[machine_id], job_ready_time[job_id]);
            int completion_time = start_time + process_time;

            if (completion_time < earliest_completion_time) {
                earliest_completion_time = completion_time;
                earliest_ops.clear();
                earliest_ops.emplace_back(job_id, op_index);
            } else if (completion_time == earliest_completion_time) {
                earliest_ops.emplace_back(job_id, op_index);
            }
        }

        // 从最早完成的工序中随机选择一个
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(0, earliest_ops.size() - 1);
        int selected_index = dist(gen);

        int selected_job = earliest_ops[selected_index].first;
        int selected_op_index = earliest_ops[selected_index].second;
        int selected_machine = instance.data[selected_job][selected_op_index].first;
        int selected_process_time = instance.data[selected_job][selected_op_index].second;

        // 计算开始时间和完成时间
        int start_time = std::max(machine_ready_time[selected_machine], job_ready_time[selected_job]);
        int completion_time = start_time + selected_process_time;

        // 更新就绪时间
        machine_ready_time[selected_machine] = completion_time;
        job_ready_time[selected_job] = completion_time;

        // 更新当前工件的工序索引
        current_job_operation[selected_job]++;

        // 更新图结构
        int operation_id = selected_job * operation_num + selected_op_index + 1;

        // 更新工件的工序边
        if (selected_op_index == 0) {
            graph.first_job_operation[selected_job] = operation_id;
        } else {
            int prev_operation_id = selected_job * operation_num + selected_op_index;
            graph.operation_edges[prev_operation_id] = operation_id;
            graph.reverse_operation_edges[operation_id] = prev_operation_id;
        }

        if (selected_op_index == operation_num - 1) {
            graph.last_job_operation[selected_job] = operation_id;
        }

        // 更新机器的工序边
        if (graph.first_machine_operation[selected_machine] == -1) {
            graph.first_machine_operation[selected_machine] = operation_id;
        } else {
            int prev_machine_op = graph.last_machine_operation[selected_machine];
            graph.machine_edges[prev_machine_op] = operation_id;
            graph.reverse_machine_edges[operation_id] = prev_machine_op;
        }

        graph.last_machine_operation[selected_machine] = operation_id;

        // 标记为已调度
        scheduled[operation_id] = true;
    }
    for (int i = 1; i <= job_num; i++) {
        graph.operation_edges[i * operation_num] = total_operations - 1;
    }
    for (int i = 0; i < job_num; i++) {
        graph.reverse_operation_edges[i * operation_num + 1] = 0;
    }

    return graph;
}