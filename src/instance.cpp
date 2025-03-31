//
// Created by qiming on 25-3-31.
//

#include <fstream>
#include <instance.h>

std::istream &operator>>(std::istream &is, Instance &instance) {
    is >> instance.job_num >> instance.operation_num;
    instance.data.resize(instance.job_num);
    for (int i = 0; i < instance.job_num; ++i) {
        for (int j = 0; j < instance.operation_num; ++j) {
            int machine_id, time;
            is >> machine_id >> time;
            instance.data[i].emplace_back(machine_id, time);
        }
    }
    return is;
}

Instance load_instance(const std::string &path) {
    Instance instance;
    try {
        std::ifstream ifs(path);
        ifs >> instance;
        return instance;
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        exit(1);
    }
}
std::ostream &operator<<(std::ostream &os, const Instance &instance) {
    os << instance.job_num << "\t" << instance.operation_num << std::endl;
    for (int i = 0; i < instance.job_num; ++i) {
        for (int j = 0; j < instance.operation_num; ++j) {
            os << instance.data[i][j].first << " " << instance.data[i][j].second << " ";
        }
        os << std::endl;
    }
    return os;
}
