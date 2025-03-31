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

struct Schedule {
    int operation_id;
    int start_time;
    int end_time;
    int machine_id;
};

class Scheduler {
public:
    explicit Scheduler(const Instance &instance) : instance(instance), operation_list(instance) {
        job_finish_time.resize(instance.job_num, 0);
        machine_available_time.resize(get_max_machine_id() + 1, 0);
    }

    std::vector<Schedule> generate_initial_solution() {
        std::vector<Schedule> schedule;

        // 遍历所有工件
        for (int job_id = 1; job_id <= instance.job_num; ++job_id) {
            int current_time = 0;// 当前工件的起始时间
            for (int operation_cnt = 1; operation_cnt <= instance.operation_num; ++operation_cnt) {
                Operation &op = operation_list.get_operation_ref(job_id, operation_cnt);

                int machine_id = op.machine_id;
                int processing_time = op.time;

                // 计算最早可行时间
                int earliest_start = std::max(job_finish_time[job_id - 1], machine_available_time[machine_id]);
                int end_time = earliest_start + processing_time;

                // 记录调度结果
                schedule.push_back({op.operation_id, earliest_start, end_time, machine_id});

                // 更新状态
                job_finish_time[job_id - 1] = end_time;
                machine_available_time[machine_id] = end_time;
            }
        }
        return schedule;
    }

private:
    const Instance &instance;
    OperationList operation_list;
    std::vector<int> job_finish_time;       // 记录每个工件的完工时间
    std::vector<int> machine_available_time;// 记录每台机器的可用时间

    int get_max_machine_id() {
        int max_id = 0;
        for (const auto &job: instance.data) {
            for (const auto &op: job) {
                max_id = std::max(max_id, op.first);
            }
        }
        return max_id;
    }
};

void save_schedule_to_csv(const std::vector<Schedule> &schedule, const std::string &filename) {
    std::ofstream file(filename);
    file << "operation_id,machine_id,start_time,end_time\n";
    for (const auto &s: schedule) {
        file << s.operation_id << "," << s.machine_id << ","
             << s.start_time << "," << s.end_time << "\n";
    }
    file.close();
}
Graph generate_initial_solution(const Instance& instance);
// 测试代码
int main() {
    Instance instance = load_instance(R"(C:\Users\qiming\CLionProjects\JobShopScheduling\instance\ft\ft06.txt)");// 需要提供数据文件
    Scheduler scheduler(instance);

    auto solution = scheduler.generate_initial_solution();
    save_schedule_to_csv(solution, R"(C:\Users\qiming\CLionProjects\JobShopScheduling\output\schedule.csv)");

    // 输出结果
    std::cout << "Initial Solution:\n";
    for (const auto &s: solution) {
        std::cout << "Operation " << s.operation_id << " on Machine " << s.machine_id
                  << " starts at " << s.start_time << ", ends at " << s.end_time << "\n";
    }
    Graph graph = generate_initial_solution(instance);


    return 0;
}
