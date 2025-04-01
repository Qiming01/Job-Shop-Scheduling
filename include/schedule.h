//
// Created by qiming on 25-4-1.
//

#ifndef JOBSHOPSCHEDULING_SCHEDULE_H
#define JOBSHOPSCHEDULING_SCHEDULE_H
#include "instance.h"
#include "operation.h"
#include "graph.h"
#include <random>
#include <algorithm>
class Schedule {
public:
    explicit Schedule(const Instance& instance) noexcept : operation_list(instance), machine_operation(instance)
    {
        MachineOperation rand_ops = machine_operation;
        // 使用随机设备生成种子
        std::random_device rd;
        std::mt19937 g(rd());


        for (auto& op_list : rand_ops.operation_ids) {
            std::shuffle(op_list.begin(), op_list.end(), g);
        }

    }

private:
    OperationList operation_list;
    MachineOperation machine_operation;
    Graph graph;
};
#endif//JOBSHOPSCHEDULING_SCHEDULE_H
