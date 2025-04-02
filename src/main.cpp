//
// Created by qiming on 25-3-31.
//
#include "graph.h"
#include "instance.h"
#include "operation.h"
#include <iostream>
#include <deque>
#include "schedule.h"

// 测试代码
int main() {
    Instance instance = load_instance(R"(../../instance/ft/ft10.txt)");
    MachineOperation machine_operation(instance);
    OperationList operation_list(instance);
    auto graph = generateRandomInitialSolution(instance);
    auto que = graph.toptoplogical_sort(true);
    for (auto i : que) {
        std::cout << i << " ";
    }
    return 0;
}
