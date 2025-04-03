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
    Instance instance = load_instance(R"(../../instance/ft/ft06.txt)");

    auto schedule = Schedule(instance);
    schedule.export_to_csv("../../output/jssp_schedule.csv");
    return 0;
}
