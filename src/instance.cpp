//
// Created by qiming on 25-3-31.
//

#include <fstream>
#include <instance.h>
#include <stdexcept>

/**
 * Overloaded input stream operator for Instance class
 * @param is Input stream
 * @param instance Instance object to populate
 * @return Reference to the input stream
 */
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

/**
 * Loads JSSP instance from file
 * @param path Path to the instance file
 * @return Instance object containing the problem data
 * @throws std::runtime_error if file cannot be opened or is invalid
 */
Instance load_instance(const std::string &path) {
    std::ifstream ifs(path);
    if (!ifs.is_open()) {
        throw std::runtime_error("Cannot open instance file: " + path);
    }

    Instance instance;
    try {
        ifs >> instance;
        if (instance.job_num <= 0 || instance.operation_num <= 0) {
            throw std::runtime_error("Invalid instance data in file: " + path);
        }
        return instance;
    } catch (const std::exception &e) {
        throw std::runtime_error("Error reading instance file: " + std::string(e.what()));
    }
}

/**
 * Overloaded output stream operator for Instance class
 * @param os Output stream
 * @param instance Instance object to output
 * @return Reference to the output stream
 */
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