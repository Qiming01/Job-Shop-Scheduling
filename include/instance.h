//
// Created by qiming on 25-3-31.
//

#ifndef JOBSHOPSCHEDULING_INSTANCE_H
#define JOBSHOPSCHEDULING_INSTANCE_H
#include <iostream>
#include <string>
#include <vector>

/**
 * 算例
 */
struct Instance {
    int job_num;      // 作业数
    int operation_num;// 操作数
    std::vector<std::vector<std::pair<int, int>>> data;
};

std::istream &operator>>(std::istream &is, Instance &instance);
std::ostream &operator<<(std::ostream &os, const Instance &instance);

/**
 * 从文件中加载算例
 * @param path 文件路径
 * @return 算例对象
 */
Instance load_instance(const std::string &path);
#endif//JOBSHOPSCHEDULING_INSTANCE_H
