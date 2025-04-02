//
// Created by qiming on 25-4-1.
//
#include "graph.h"
#include <cassert>

bool test_toptoplogical_sort() {
    Graph graph;
    graph.job_num = 3;
    graph.operation_num = 3;
    graph.first_job_operation = { 1, 4, 7 };
    graph.first_machine_operation = { 4, 5, 7 };
    graph.last_job_operation = { 3, 6, 9 };
    graph.last_machine_operation = { 3, 6, 8 };
    graph.machine_edges = { -1, 8, 6, -1, 1, 9, -1, 2, -1, 3, -1 };
    graph.operation_edges = { -1, 2, 3, 10, 5, 6, 10, 8, 9, 10, -1 };
    graph.reverse_machine_edges = { -1, 4, 7, 9, -1, -1, 2, -1, 1, 5, -1 };
    graph.reverse_operation_edges = { -1, 0, 1, 2, 0, 4, 5, 0, 7, 8, -1 };
    auto que = graph.toptoplogical_sort();
    std::deque<int> check_que { 0, 4, 7, 5, 1, 2, 8, 6, 9, 3, 10};
    assert(que == check_que);
    que = graph.toptoplogical_sort(true);
    check_que = { 10, 3, 6, 9, 2, 8, 5, 7, 1, 4, 0};
    assert(que == check_que);
    return true;
}
std::deque<int> Graph::toptoplogical_sort(bool reverse) const {
    int n = job_num * operation_num + 2;
    std::vector<int> in_degree(n, 0);
    int first_node = reverse ? n - 1 : 0;
    // 计算入度
    for (int i = 0; i < n; ++i) {
        // 逆向拓扑排序的时候，入度等于正向边中-1的数量
        if (reverse) {
            in_degree[i] = (machine_edges[i] != -1) + (operation_edges[i] != -1);
        } else {
            in_degree[i] = (int)(reverse_machine_edges[i] != -1) + (int)(reverse_operation_edges[i] != -1);
        }
    }
    std::deque<int> result; // 存储拓扑排序的结果
    std::deque<int> candidates; // 存储入度为0的节点
    candidates.push_back(first_node);
    while (true) {
        if (candidates.empty()) {
            // 如果入度为0的节点为空，则说明存在环
            throw std::runtime_error("Graph contains a cycle");
        }
        int curr = candidates.front();  // 获取入度为0的节点
        candidates.pop_front();
        result.push_back(curr);     // 将节点加入结果中
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
                for (const int node : last_job_operation)
                {
                    in_degree[node]--;
                    if (in_degree[node] == 0)
                    {
                        candidates.push_back(node);
                    }
                }
            } else {
                for (const int node : first_job_operation)
                {
                    in_degree[node]--;
                    if (in_degree[node] == 0)
                    {
                        candidates.push_back(node);
                    }
                }
            }
        } else {
            if (reverse) {
                if (reverse_operation_edges[curr] != -1)
                {
                    in_degree[reverse_operation_edges[curr]]--;
                    if (in_degree[reverse_operation_edges[curr]] == 0)
                    {
                        candidates.push_back(reverse_operation_edges[curr]);
                    }
                }

                if (reverse_machine_edges[curr] != -1)
                {
                    in_degree[reverse_machine_edges[curr]]--;
                    if (in_degree[reverse_machine_edges[curr]] == 0)
                    {
                        candidates.push_back(reverse_machine_edges[curr]);
                    }
                }
            } else {
                if (operation_edges[curr] != -1)
                {
                    in_degree[operation_edges[curr]]--;
                    if (in_degree[operation_edges[curr]] == 0)
                    {
                        candidates.push_back(operation_edges[curr]);
                    }
                }

                if (machine_edges[curr] != -1)
                {
                    in_degree[machine_edges[curr]]--;
                    if (in_degree[machine_edges[curr]] == 0)
                    {
                        candidates.push_back(machine_edges[curr]);
                    }
                }
            }
        }
    }
    return result;
}
