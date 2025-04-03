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

void Schedule::update_critical_blocks() {
    critical_blocks.clear();
    for (auto start_machine_op : graph.first_machine_operation) {
        int curr_machine_op = start_machine_op;
        std::vector<int> critical_block;
        // critical_block.reserve(graph.operation_num);
        while (curr_machine_op != -1) {
            // 如果当前块是critical block
            if (is_critical_operation(curr_machine_op)) {
                // 如果关键块为空，则直接加入
                if (critical_block.empty()) {
                    critical_block.push_back(curr_machine_op);
                } else {
                    int prev_critical_op = critical_block.back();
                    // 如果前一个关键块的结束时间等于当前关键块的开始时间，则加入
                    if (time_info[prev_critical_op].end_time == time_info[curr_machine_op].forward_path_length) {
                        critical_block.push_back(curr_machine_op);
                    } else {
                        if (critical_block.size() > 1) {
                            critical_blocks.push_back(critical_block);
                        }
                        critical_block.clear();
                        critical_block.push_back(curr_machine_op);
                    }
                }
            } else {
                if (critical_block.size() > 1) {
                    critical_blocks.push_back(critical_block);
                    critical_block.clear();
                }
            }
            curr_machine_op = graph.machine_edges[curr_machine_op];
        }
        if (critical_block.size() > 1) {
            critical_blocks.push_back(critical_block);
        }
    }
}

/**
 * 将调度结果导出为CSV文件
 * @param filename 导出文件名
 */
void Schedule::export_to_csv(const std::string &filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }

    // 输出表头
    file << "ID,Job,Operation,Machine,StartTime,EndTime,IsCritical" << std::endl;
    for (int i = 1; i < graph.node_num - 1; i++) {
        file << i << ","
             << operation_list[i].job_id << ","
             << operation_list[i].operation_cnt << ","
             << operation_list[i].machine_id << ","
             << time_info[i].forward_path_length << ","
             << time_info[i].end_time << ","
             << is_critical_operation(i) << std::endl;
    }

    file.close();
    std::clog << "Schedule exported to " << filename << std::endl;
}