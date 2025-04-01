//
// Created by qiming on 25-3-31.
//
#include "graph.h"
#include "instance.h"
#include "operation.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <limits>
#include <vector>

Graph generate_initial_solution(const Instance& instance);
// 测试代码
int main() {
    Instance instance = load_instance(R"(../../instance/simple/3.txt)");
    MachineOperation machine_operation(instance);
    OperationList operation_list(instance);
    return 0;
}
