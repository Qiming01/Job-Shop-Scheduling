//
// Created by qiming on 25-3-31.
//
#include "graph.h"
#include "instance.h"
#include "operation.h"
#include "schedule.h"
#include <deque>
#include <iostream>

// 测试代码
int main() {
    Instance instance = load_instance(R"(../../instance/simple/3.txt)");
    // MachineOperation machine_operation(instance);
    // OperationList operation_list(instance);
    // auto graph = generate_random_initial_solution(instance);
    // // 计算调度时间
    // auto schedule = calculateScheduleTimes(graph, operation_list);
    //
    // // 导出为CSV
    // exportToCSV(schedule, "../../output/jssp_schedule.csv");
    auto schedule = Schedule(instance);
    return 0;
}
