//
// Created by qiming on 25-4-1.
//
#include "graph.h"
#include <cassert>
#include <random>

bool test_topological_sort() {
    Graph graph;
    graph.job_num = 3;
    graph.operation_num = 3;
    graph.first_job_operation = {1, 4, 7};
    graph.first_machine_operation = {4, 5, 7};
    graph.last_job_operation = {3, 6, 9};
    graph.last_machine_operation = {3, 6, 8};
    graph.machine_edges = {-1, 8, 6, -1, 1, 9, -1, 2, -1, 3, -1};
    graph.operation_edges = {-1, 2, 3, 10, 5, 6, 10, 8, 9, 10, -1};
    graph.reverse_machine_edges = {-1, 4, 7, 9, -1, -1, 2, -1, 1, 5, -1};
    graph.reverse_operation_edges = {-1, 0, 1, 2, 0, 4, 5, 0, 7, 8, -1};
    auto que = graph.topological_sort();
    std::deque<int> check_que{0, 4, 7, 5, 1, 2, 8, 6, 9, 3, 10};
    assert(que == check_que);
    que = graph.topological_sort(true);
    check_que = {10, 3, 6, 9, 2, 8, 5, 7, 1, 4, 0};
    assert(que == check_que);
    return true;
}
std::deque<int> Graph::topological_sort(bool reverse) const {
    int n = job_num * operation_num + 2;
    std::vector<int> in_degree(n, 0);
    int first_node = reverse ? n - 1 : 0;
    // 计算入度
    for (int i = 0; i < n; ++i) {
        // 逆向拓扑排序的时候，入度等于正向边中-1的数量
        if (reverse) {
            in_degree[i] = (machine_edges[i] != -1) + (operation_edges[i] != -1);
        } else {
            in_degree[i] = (int) (reverse_machine_edges[i] != -1) + (int) (reverse_operation_edges[i] != -1);
        }
    }
    std::deque<int> result;    // 存储拓扑排序的结果
    std::deque<int> candidates;// 存储入度为0的节点
    candidates.push_back(first_node);
    while (true) {
        if (candidates.empty()) {
            // 如果入度为0的节点为空，则说明存在环
            throw std::runtime_error("Graph contains a cycle");
        }
        int curr = candidates.front();// 获取入度为0的节点
        candidates.pop_front();
        result.push_back(curr);// 将节点加入结果中
        if (result.size() == n - 1) {
            // 加入最后一个节点
            if (reverse) {
                result.push_back(0);
            } else {
                result.push_back(n - 1);
            }
            break;
        }

        if (curr == first_node) {
            // 如果是第一个结点，单独处理
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
                if (reverse_operation_edges[curr] != -1) {
                    in_degree[reverse_operation_edges[curr]]--;
                    if (in_degree[reverse_operation_edges[curr]] == 0) {
                        candidates.push_back(reverse_operation_edges[curr]);
                    }
                }

                if (reverse_machine_edges[curr] != -1) {
                    in_degree[reverse_machine_edges[curr]]--;
                    if (in_degree[reverse_machine_edges[curr]] == 0) {
                        candidates.push_back(reverse_machine_edges[curr]);
                    }
                }
            } else {
                if (operation_edges[curr] != -1) {
                    in_degree[operation_edges[curr]]--;
                    if (in_degree[operation_edges[curr]] == 0) {
                        candidates.push_back(operation_edges[curr]);
                    }
                }

                if (machine_edges[curr] != -1) {
                    in_degree[machine_edges[curr]]--;
                    if (in_degree[machine_edges[curr]] == 0) {
                        candidates.push_back(machine_edges[curr]);
                    }
                }
            }
        }
    }
    return result;
}


/**
 * 使用Giffler-Thompson算法生成JSSP问题的随机初始解
 * @param instance JSSP问题实例
 * @return 生成的调度图
 */
Graph generate_random_initial_solution(const Instance &instance) {
    int job_num = instance.job_num;
    int operation_num = instance.operation_num;

    // 初始化调度图
    Graph graph;
    graph.job_num = job_num;
    graph.operation_num = operation_num;
    graph.node_num = job_num * operation_num + 2;

    // 初始化边
    int total_operations = job_num * operation_num + 2;// 包含虚拟头尾节点
    graph.operation_edges.resize(total_operations, -1);
    graph.machine_edges.resize(total_operations, -1);
    graph.reverse_operation_edges.resize(total_operations, -1);
    graph.reverse_machine_edges.resize(total_operations, -1);

    // 初始化工件的第一个和最后一个工序
    graph.first_job_operation.resize(job_num, -1);
    graph.last_job_operation.resize(job_num, -1);
    graph.first_machine_operation.resize(operation_num, -1);
    graph.last_machine_operation.resize(operation_num, -1);


    // Giffler-Thompson算法实现
    // 1. 初始化
    std::vector<bool> scheduled(job_num * operation_num + 1, false);// 标记哪些工序已经被调度
    std::vector<int> machine_ready_time(operation_num, 0);          // 每台机器的就绪时间
    std::vector<int> job_ready_time(job_num, 0);                    // 每个工件的就绪时间

    // 工件的当前工序索引
    std::vector<int> current_job_operation(job_num, 0);

    // 2. 主循环：直到所有工序都被调度
    for (int scheduled_count = 0; scheduled_count < job_num * operation_num; scheduled_count++) {
        // 找出所有可以调度的工序
        std::vector<std::pair<int, int>> candidates;// (job_id, op_index)

        // 遍历所有的工件
        for (int job_id = 0; job_id < job_num; job_id++) {
            int op_index = current_job_operation[job_id];// 当前工件执行到哪一个工序了
            if (op_index < operation_num) {              // 还有工序未被调度
                candidates.emplace_back(job_id, op_index);
            }
        }

        // 找出最早完成的工序
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


/**
 * 计算调度图中每个工序的开始时间和结束时间
 * @param graph 调度图
 * @param OperationList 操作集合
 * @return 包含每个工序调度信息的向量
 */
std::vector<StartTimeInfo> calculateScheduleTimes(const Graph &graph, const OperationList &op_list) {
    std::vector<StartTimeInfo> schedule(graph.job_num * graph.operation_num + 2);
    auto sequence = graph.topological_sort();
    for (auto op_id: sequence) {
        int start_time = 0;
        int prev_op_id = graph.reverse_operation_edges[op_id];
        int prev_machine_id = graph.reverse_machine_edges[op_id];
        // 如果该工件有前一个同工件的操作，则更新开始时间
        if (prev_op_id != -1) {
            start_time = schedule[prev_op_id].end_time;
        }
        // 如果该工件有前一个同机器的操作，则更新开始时间
        if (prev_machine_id != -1) {
            start_time = std::max(start_time, schedule[prev_machine_id].end_time);
        }
        schedule[op_id].start_time = start_time;
        schedule[op_id].end_time = schedule[op_id].start_time + op_list[op_id].time;
        schedule[op_id].job_id = op_list[op_id].job_id;
        schedule[op_id].machine_id = op_list[op_id].machine_id;
        schedule[op_id].operation_id = op_id;
        schedule[op_id].operation_cnt = op_list[op_id].operation_cnt;
    }
    return schedule;
}

/**
 * 将调度结果导出为CSV文件
 * @param schedule 调度结果
 * @param filename 导出文件名
 */
void exportToCSV(const std::vector<StartTimeInfo> &schedule, const std::string &filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    // 写入CSV头
    file << "id,job,op,machine,start,end\n";

    // 写入数据
    for (const auto &op: schedule) {
        if (op.operation_id == 0 || op.operation_id == schedule.size() - 1) {
            continue;
        }
        file << op.operation_id << ","
             << op.job_id << ","
             << op.operation_cnt << ","
             << op.machine_id << ","
             << op.start_time << ","
             << op.end_time << "\n";
    }
    file.close();
    std::cout << "Schedule exported to " << filename << std::endl;
}