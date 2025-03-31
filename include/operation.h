//
// Created by qiming on 25-3-31.
//

#ifndef JOBSHOPSCHEDULING_OPERATION_H
#define JOBSHOPSCHEDULING_OPERATION_H
#include "instance.h"
#include <vector>
struct Operation {
    int operation_id; // 工序id
    int operation_cnt;// 第几道工序
    int machine_id;   // 需要在哪一个机器上完成
    int time;         // 执行时间

    Operation(int operation_id, int operation_cnt, int machine_id, int time)
        : operation_id(operation_id), operation_cnt(operation_cnt), machine_id(machine_id), time(time) {}
};

struct OperationList {
    int job_num;
    int operation_num;
    std::vector<Operation> operations;

    explicit OperationList(Instance instance) noexcept : job_num(instance.job_num), operation_num(instance.operation_num) {
        int operation_id = 0;
        operations.emplace_back(operation_id++, 0, 0, 0);// 头节点
        for (int i = 0; i < instance.job_num; ++i) {
            for (int j = 0; j < instance.operation_num; ++j) {
                operations.emplace_back(operation_id++, j + 1, instance.data[i][j].first, instance.data[i][j].second);
            }
        }
        operations.emplace_back(operation_id++, 0, 0, 0);// 尾节点
    }

    Operation &operator[](int id) {
        return operations[id];
    }

    /**
     * 获取指定job_id的第operation_cnt个工序
     * @param job_id 工件id
     * @param operation_cnt 该工件的第几个工序
     * @return 该工序的操作
     */
    Operation &get_operation_ref(int job_id, int operation_cnt) {
        return operations[(job_id - 1) * operation_num + operation_cnt];
    }

    /**
     * 获取指定operation_id的工序
     * @param operation_id 工序id
     * @return 该工序的操作
     */
    Operation &get_operation_ref(int operation_id) {
        return operations[operation_id];
    }
};
#endif//JOBSHOPSCHEDULING_OPERATION_H
