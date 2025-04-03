//
// Created by qiming on 25-3-31.
//

#ifndef JOBSHOPSCHEDULING_GRAPH_H
#define JOBSHOPSCHEDULING_GRAPH_H

#include "operation.h"
#include <deque>
#include <fstream>
#include <vector>
/**
 * 析取图
 * 总结点数 = job_num * operation_num + 2
 */
struct Graph {
    int job_num;      // 工件数
    int operation_num;// 每个工件需要的工序数
    int node_num;     // job_num * operation_num + 2

    /**
     * 析取图的正向的边
     * 除了虚拟结点外，edges[u] = v 表示 u -> v 有一条边
     * edges[u] = -1 表示 u 没有出边
     * 由于只能保存一条边的id，因此虚拟头节点的下一个结点无法表示，
     * 因此单独处理，使用first_job_operation、first_machine_operation表示虚拟节点的下一个结点
     */
    std::vector<int> operation_edges;// 工件的下一个工序的id
    std::vector<int> machine_edges;  // 同机器的下一个工序的id

    std::vector<int> reverse_operation_edges;// 反向边
    std::vector<int> reverse_machine_edges;  // 反向机器边

    /**
     * 用于处理虚拟节点的下一个结点无法表示的问题
     */
    std::vector<int> first_job_operation;    // 每个工件的第一个工序id
    std::vector<int> last_job_operation;     // 每个工件的最后一个工序id
    std::vector<int> first_machine_operation;// 每台机器的第一个工序id
    std::vector<int> last_machine_operation; // 每台机器的最后一个工序id

    [[nodiscard]] std::deque<int> topological_sort(bool reverse = false) const;
};


Graph generate_random_initial_solution(const Instance &instance);
bool test_topological_sort();

// 计算开始和结束时间的结构体
struct StartTimeInfo {
    int operation_id;
    int job_id;
    int operation_cnt;
    int machine_id;
    int start_time;
    int end_time;
};

/**
 * 计算调度图中每个工序的开始时间和结束时间
 * @param graph 调度图
 * @param OperationList 操作集合
 * @return 包含每个工序调度信息的向量
 */
std::vector<StartTimeInfo> calculateScheduleTimes(const Graph &graph, const OperationList &op_list);

/**
 * 将调度结果导出为CSV文件
 * @param schedule 调度结果
 * @param filename 导出文件名
 */
void exportToCSV(const std::vector<StartTimeInfo> &schedule, const std::string &filename);

#endif//JOBSHOPSCHEDULING_GRAPH_H
