//
// Created by qiming on 25-4-1.
//
#include "graph.h"
#include "RandomGenerator.h"
#include <cassert>
#include <random>

/**
 * Perform topological sort on the graph
 * @param reverse If true, perform reverse topological sort
 * @return A deque containing nodes in topologically sorted order
 * @throws std::runtime_error if graph contains a cycle
 */
std::deque<int> Graph::topological_sort(bool reverse) const {
    int n = job_num * operation_num + 2;
    std::vector<int> in_degree(n, 0);
    int first_node = reverse ? n - 1 : 0;

    // Calculate in-degree for each node
    for (int i = 0; i < n; ++i) {
        if (reverse) {
            in_degree[i] = (machine_successor[i] != -1) + (job_successor[i] != -1);
        } else {
            in_degree[i] = (int) (machine_predecessor[i] != -1) + (int) (job_predecessor[i] != -1);
        }
    }

    std::deque<int> result;    // Topological order result
    std::deque<int> candidates;// Nodes with zero in-degree
    candidates.push_back(first_node);

    while (true) {
        if (candidates.empty()) {
            throw std::runtime_error("Graph contains a cycle");
        }

        int curr = candidates.front();
        candidates.pop_front();
        result.push_back(curr);

        if (result.size() == n - 1) {
            if (reverse) {
                result.push_back(0);
            } else {
                result.push_back(n - 1);
            }
            break;
        }

        if (curr == first_node) {
            if (reverse) {
                for (const int node: last_job_operation) {
                    in_degree[node]--;
                    if (in_degree[node] == 0) {
                        candidates.push_back(node);
                    }
                }
            } else {
                for (const int node: first_job_operation) {
                    in_degree[node]--;
                    if (in_degree[node] == 0) {
                        candidates.push_back(node);
                    }
                }
            }
        } else {
            if (reverse) {
                if (job_predecessor[curr] != -1) {
                    in_degree[job_predecessor[curr]]--;
                    if (in_degree[job_predecessor[curr]] == 0) {
                        candidates.push_back(job_predecessor[curr]);
                    }
                }

                if (machine_predecessor[curr] != -1) {
                    in_degree[machine_predecessor[curr]]--;
                    if (in_degree[machine_predecessor[curr]] == 0) {
                        candidates.push_back(machine_predecessor[curr]);
                    }
                }
            } else {
                if (job_successor[curr] != -1) {
                    in_degree[job_successor[curr]]--;
                    if (in_degree[job_successor[curr]] == 0) {
                        candidates.push_back(job_successor[curr]);
                    }
                }

                if (machine_successor[curr] != -1) {
                    in_degree[machine_successor[curr]]--;
                    if (in_degree[machine_successor[curr]] == 0) {
                        candidates.push_back(machine_successor[curr]);
                    }
                }
            }
        }
    }
    return result;
}

/**
 * Generates a random initial solution for JSSP using Giffler-Thompson algorithm
 * @param instance The JSSP problem instance
 * @return A Graph representing the generated schedule
 */
Graph generate_random_initial_solution(const Instance &instance) {
    int job_num = instance.job_num;
    int operation_num = instance.operation_num;

    // Initialize graph
    Graph graph;
    graph.job_num = job_num;
    graph.operation_num = operation_num;
    graph.node_num = job_num * operation_num + 2;

    // Initialize edges
    int total_operations = job_num * operation_num + 2;
    graph.job_successor.resize(total_operations, -1);
    graph.machine_successor.resize(total_operations, -1);
    graph.job_predecessor.resize(total_operations, -1);
    graph.machine_predecessor.resize(total_operations, -1);

    // Initialize operation tracking
    graph.first_job_operation.resize(job_num, -1);
    graph.last_job_operation.resize(job_num, -1);
    graph.first_machine_operation.resize(operation_num, -1);
    graph.last_machine_operation.resize(operation_num, -1);

    // Giffler-Thompson algorithm
    std::vector<bool> scheduled(job_num * operation_num + 1, false);
    std::vector<int> machine_ready_time(operation_num, 0);
    std::vector<int> job_ready_time(job_num, 0);
    std::vector<int> current_job_operation(job_num, 0);

    // Main scheduling loop
    for (int scheduled_count = 0; scheduled_count < job_num * operation_num; scheduled_count++) {
        // Find all schedulable operations
        std::vector<std::pair<int, int>> candidates;

        for (int job_id = 0; job_id < job_num; job_id++) {
            int op_index = current_job_operation[job_id];
            if (op_index < operation_num) {
                candidates.emplace_back(job_id, op_index);
            }
        }

        // Find earliest completing operations
        int earliest_completion_time = INT_MAX;
        std::vector<std::pair<int, int>> earliest_ops;

        for (auto &candidate: candidates) {
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

        // Randomly select from the earliest operations
        int selected_index = RandomGenerator::instance().getInt(static_cast<int>(earliest_ops.size()));

        int selected_job = earliest_ops[selected_index].first;
        int selected_op_index = earliest_ops[selected_index].second;
        int selected_machine = instance.data[selected_job][selected_op_index].first;
        int selected_process_time = instance.data[selected_job][selected_op_index].second;

        // Update schedule
        int start_time = std::max(machine_ready_time[selected_machine], job_ready_time[selected_job]);
        int completion_time = start_time + selected_process_time;

        machine_ready_time[selected_machine] = completion_time;
        job_ready_time[selected_job] = completion_time;
        current_job_operation[selected_job]++;

        // Update graph edges
        int operation_id = selected_job * operation_num + selected_op_index + 1;

        // Update job edges
        if (selected_op_index == 0) {
            graph.first_job_operation[selected_job] = operation_id;
        } else {
            int prev_operation_id = selected_job * operation_num + selected_op_index;
            graph.job_successor[prev_operation_id] = operation_id;
            graph.job_predecessor[operation_id] = prev_operation_id;
        }

        if (selected_op_index == operation_num - 1) {
            graph.last_job_operation[selected_job] = operation_id;
        }

        // Update machine edges
        if (graph.first_machine_operation[selected_machine] == -1) {
            graph.first_machine_operation[selected_machine] = operation_id;
        } else {
            int prev_machine_op = graph.last_machine_operation[selected_machine];
            graph.machine_successor[prev_machine_op] = operation_id;
            graph.machine_predecessor[operation_id] = prev_machine_op;
        }

        graph.last_machine_operation[selected_machine] = operation_id;
        scheduled[operation_id] = true;
    }

    // Connect virtual start/end nodes
    for (int i = 1; i <= job_num; i++) {
        graph.job_successor[i * operation_num] = total_operations - 1;
    }
    for (int i = 0; i < job_num; i++) {
        graph.job_predecessor[i * operation_num + 1] = 0;
    }

    return graph;
}