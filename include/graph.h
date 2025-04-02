//
// Created by qiming on 25-3-31.
//

#ifndef JOBSHOPSCHEDULING_GRAPH_H
#define JOBSHOPSCHEDULING_GRAPH_H

#include "operation.h"
#include <fstream>
#include <vector>
#include <deque>
/**
 * 析取图
 * 总结点数 = job_num * operation_num + 2
 */
struct Graph {
    int job_num;      // 工件数
    int operation_num;// 每个工件需要的工序数

    /**
     * 析取图的正向的边
     * 除了虚拟结点外，edges[u] = v 表示 u -> v 有一条边
     * edges[u] = -1 表示 u 没有出边
     * 由于只能保存一条边的id，因此虚拟头节点的下一个结点无法表示，
     * 因此单独处理，使用first_job_operation、first_machine_operation表示虚拟节点的下一个结点
     */
    std::vector<int> operation_edges;    // 工件的下一个工序的id
    std::vector<int> machine_edges;      // 同机器的下一个工序的id

    std::vector<int> reverse_operation_edges;   // 反向边
    std::vector<int> reverse_machine_edges;     // 反向机器边

    /**
     * 用于处理虚拟节点的下一个结点无法表示的问题
     */
    std::vector<int> first_job_operation;   // 每个工件的第一个工序id
    std::vector<int> last_job_operation;    // 每个工件的最后一个工序id
    std::vector<int> first_machine_operation;// 每台机器的第一个工序id
    std::vector<int> last_machine_operation;  // 每台机器的最后一个工序id

    std::deque<int> toptoplogical_sort(bool reverse = false) const;
};

bool test_toptoplogical_sort();
#endif//JOBSHOPSCHEDULING_GRAPH_H
