//
// Created by qiming on 25-3-31.
//

#ifndef JOBSHOPSCHEDULING_GRAPH_H
#define JOBSHOPSCHEDULING_GRAPH_H

#include "operation.h"
#include <vector>
/**
 * 析取图
 */
class Graph {
    int job_num;      // 工件数
    int operation_num;// 工序数

    std::vector<int> adj_job_edges;    // 表示下一个有边的同工件工序的id
    std::vector<int> adj_machine_edges;// 表示下一个有边的同机器工序的id

    std::vector<int> reverse_adj_job_edges;
    std::vector<int> reverse_adj_machine_edges;

    std::vector<int> first_op_in_machine;// 每台机器的第一个工序id
    std::vector<int> last_op_in_machine; // 每台机器的最后一个工序id
};
#endif//JOBSHOPSCHEDULING_GRAPH_H
