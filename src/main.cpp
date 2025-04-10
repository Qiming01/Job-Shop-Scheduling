//
// Created by qiming on 25-3-31.
//
#include "instance.h"
#include "tabu_search.h"
#include <chrono>

using namespace std::chrono;

int main() {
    int limit_time = 100;// 运行时间限制
    int best_answer = 930;
    unsigned long long max_iteration = 100000000;
    int rand_seed = 2001;
    const std::string instance_path = R"(../../instance/ft/ft10.txt)";

    Instance instance = load_instance(instance_path);

    RandomGenerator::instance().setSeed(rand_seed);
    auto start = std::chrono::high_resolution_clock::now();
    auto tabuSearch = TabuSearch(instance);
    steady_clock::time_point endTime = steady_clock::now() + seconds(limit_time);
    tabuSearch.search(max_iteration,
                      [&]() { return duration_cast<milliseconds>(endTime - steady_clock::now()).count() <= 0 || tabuSearch.makespan() == best_answer; });
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::clog << "Function took " << duration.count() << " milliseconds to execute." << std::endl;
    return 0;
}
