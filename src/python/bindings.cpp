//
// Created by qiming on 25-5-28.
//
#include "tabu_search.h"        // 你的 TabuSearch 类头文件
#include <pybind11/chrono.h>    // 用于时间类型转换
#include <pybind11/functional.h>// 用于回调函数支持
#include <pybind11/pybind11.h>

namespace py = pybind11;

// 将 TabuSearch 类绑定到 Python
void bind_tabu_search(py::module_ &m) {
    py::class_<TabuSearch>(m, "TabuSearch")
            .def(py::init<const Instance &>())
            .def("search", &TabuSearch::search,
                 py::arg("max_iteration") = 1e8,
                 py::arg("stop_condition"))
            .def("search_for_seconds", [](TabuSearch &ts, int seconds, unsigned long long max_iter = 1e8) {
                auto end_time = std::chrono::steady_clock::now() + std::chrono::seconds(seconds);
                ts.search(max_iter, [&]() {
                    return std::chrono::steady_clock::now() >= end_time;
                }); }, py::arg("seconds"), py::arg("max_iteration") = 1e8)
            .def("makespan", &TabuSearch::makespan)
            .def("get_iter", &TabuSearch::get_iter)
            .def("export_result", &TabuSearch::export_result)
            .def("__repr__", [](const TabuSearch &ts) { return "<TabuSearch>"; });

    m.def("set_seed", [](int seed) { RandomGenerator::instance().setSeed(seed); }, py::arg("seed"));
}

// 绑定 Instance 相关功能
void bind_instance(py::module_ &m) {
    // 绑定 Instance 类型
    py::class_<Instance>(m, "Instance")
            .def_readwrite("job_num", &Instance::job_num)
            .def_readwrite("operation_num", &Instance::operation_num)
            .def_readwrite("data", &Instance::data);// std::vector<std::vector<std::pair<int, int>>>

    // 绑定加载函数
    m.def("load_instance", &load_instance,
          py::arg("instance_path"));
}

// 主模块定义
PYBIND11_MODULE(jsp, m) {
    m.doc() = "Job Shop Scheduling Python Bindings";
    // py::class_<std::chrono::steady_clock::time_point>(m, "TimePoint");
    bind_instance(m);
    bind_tabu_search(m);
}