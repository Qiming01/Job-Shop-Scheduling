//
// Created by qiming on 25-3-31.
//
#include "schedule.h"

/**
 * Calculates forward and backward scheduling information
 * Computes R value (forward pass) and Q value (backward pass)
 * Also calculates the overall makespan of the schedule
 */
void Schedule::calculate_time_info() {
    int n = graph.node_num;
    auto forward_queue = graph.topological_sort(false);// Forward topological order
    auto backward_queue = graph.topological_sort(true);// Reverse topological order

    // Forward pass to calculate the earliest start times (R values)
    time_info.resize(n);
    makespan = 0;
    for (int i = 1; i < n - 1; i++) {
        int curr_node = forward_queue[i];
        int start_time = 0;
        int prev_op_id = graph.job_predecessor[curr_node];
        int prev_machine_id = graph.machine_predecessor[curr_node];

        // Consider job predecessor constraints
        if (prev_op_id != -1) {
            start_time = time_info[prev_op_id].end_time;
        }

        // Consider machine predecessor constraints
        if (prev_machine_id != -1) {
            start_time = std::max(start_time, time_info[prev_machine_id].end_time);
        }

        // Update node time information
        time_info[curr_node].operator_id = curr_node;
        time_info[curr_node].forward_path_length = start_time;
        int end_time = start_time + operation_list[curr_node].time;

        // Update makespan if needed
        if (end_time > makespan) {
            makespan = end_time;
        }
        time_info[curr_node].end_time = end_time;
    }

    // Backward pass to calculate Q values
    for (int i = 1; i < n - 1; i++) {
        int curr_node = backward_queue[i];
        int next_node_time = 0;

        // Consider job successor constraints
        if (graph.job_successor[curr_node] != -1) {
            next_node_time = std::max(time_info[graph.job_successor[curr_node]].backward_path_length, next_node_time);
        }

        // Consider machine successor constraints
        if (graph.machine_successor[curr_node] != -1) {
            next_node_time = std::max(time_info[graph.machine_successor[curr_node]].backward_path_length, next_node_time);
        }

        time_info[curr_node].backward_path_length = next_node_time + operation_list[curr_node].time;
    }
}

/**
 * Identifies and updates critical blocks in the schedule
 * A critical block is a sequence of operations where:
 * 1. Each operation is critical (on the critical path)
 * 2. Operations are consecutive on the same machine
 * 3. There is no idle time between operations
 */
void Schedule::update_critical_blocks() {
    critical_blocks.clear();

    // Process each machine's operation sequence
    for (auto start_machine_op: graph.first_machine_operation) {
        int curr_machine_op = start_machine_op;
        std::vector<int> critical_block;

        while (curr_machine_op != -1) {
            if (is_critical_operation(curr_machine_op)) {
                if (critical_block.empty()) {
                    critical_block.push_back(curr_machine_op);
                } else {
                    int prev_critical_op = critical_block.back();

                    // Check for continuous critical operations
                    if (time_info[prev_critical_op].end_time == time_info[curr_machine_op].forward_path_length) {
                        critical_block.push_back(curr_machine_op);
                    } else {
                        // Save block if valid and start new one
                        if (critical_block.size() > 1) {
                            critical_blocks.push_back(critical_block);
                        }
                        critical_block.clear();
                        critical_block.push_back(curr_machine_op);
                    }
                }
            } else {
                // Non-critical operation breaks current block
                if (critical_block.size() > 1) {
                    critical_blocks.push_back(critical_block);
                }
                critical_block.clear();
            }
            curr_machine_op = graph.machine_successor[curr_machine_op];
        }

        // Save remaining operations in current block
        if (critical_block.size() > 1) {
            critical_blocks.push_back(critical_block);
        }
    }
}

/**
 * Exports schedule information to CSV file
 * @param filename Path to output CSV file
 * @throws std::runtime_error if file cannot be opened
 */
void Schedule::export_to_csv(const std::string &filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    // Write CSV header
    file << "ID,Job,Operation,Machine,StartTime,EndTime,IsCritical" << std::endl;

    // Write operation data
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