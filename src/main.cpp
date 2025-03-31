//
// Created by qiming on 25-3-31.
//
#include "instance.h"
#include "operation.h"
#include <iostream>
int main() {
    auto instance = load_instance(R"(C:\Users\qiming\CLionProjects\JobShopScheduling\instance\la\la26.txt)");

    auto operation_list = OperationList(instance);

    std::cout << operation_list.job_num << std::endl;
    return 0;
}