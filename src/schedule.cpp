//
// Created by qiming on 25-3-31.
//
#include "schedule.h"

void Schedule::calculate_time_info() {
    int n = graph.node_num;
    auto forward_queue = graph.topological_sort(false);
    auto backward_queue = graph.topological_sort(true);
    // 正向计算R值
    time_info.resize(n);
    for (int i = 1; i < n - 1; i++) {
        int curr_node = forward_queue[i];
        int start_time = 0;
        int prev_op_id = graph.reverse_operation_edges[curr_node];
        int prev_machine_id = graph.reverse_machine_edges[curr_node];
        // 如果该工件有前一个同工件的操作，则更新开始时间
        if (prev_op_id != -1) {
            start_time = time_info[prev_op_id].end_time;
        }
        // 如果该工件有前一个同机器的操作，则更新开始时间
        if (prev_machine_id != -1) {
            start_time = std::max(start_time, time_info[prev_machine_id].end_time);
        }
        time_info[curr_node].operator_id = curr_node;
        time_info[curr_node].forward_path_length = start_time;
        int end_time = start_time + operation_list[curr_node].time;
        if (end_time > makespan) {
            makespan = end_time;
        }
        time_info[curr_node].end_time = end_time;
    }
    // 反向计算Q值
    for (int i = 1; i < n - 1; i++) {
        int curr_node = backward_queue[i];
        int next_node_time = 0;
        if (graph.operation_edges[curr_node] != -1) {
            next_node_time = std::max(time_info[graph.operation_edges[curr_node]].backward_path_length, next_node_time);
        }
        if (graph.machine_edges[curr_node] != -1) {
            next_node_time = std::max(time_info[graph.machine_edges[curr_node]].backward_path_length, next_node_time);
        }
        time_info[curr_node].backward_path_length = next_node_time + operation_list[curr_node].time;
    }
}
void Schedule::update_critical_operations() {
    critical_operations.clear();
    int n = graph.node_num;
    for (int i = 1; i < n - 1; i++) {
        if (time_info[i].forward_path_length + time_info[i].backward_path_length == makespan) {
            critical_operations.push_back(i);
        }
    }
    std::sort(critical_operations.begin(), critical_operations.end(), [&](const int a, const int b) {
        return time_info[a] < time_info[b];
    });
}
void Schedule::update_critical_blocks() {
    critical_blocks.clear();
    int prev_machine_id = -1;
    std::vector<int> same_machine_op;
    for (auto op: critical_operations) {
        int curr_machine_id = operation_list[op].machine_id;
        if (curr_machine_id == prev_machine_id) {
            same_machine_op.push_back(op);
        } else {
            if (same_machine_op.size() > 1) {
                critical_blocks.push_back(same_machine_op);
            }
            same_machine_op.clear();
            same_machine_op.push_back(op);
        }
        prev_machine_id = curr_machine_id;
    }
}