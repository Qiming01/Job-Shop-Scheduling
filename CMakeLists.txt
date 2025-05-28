cmake_minimum_required(VERSION 3.30)
project(JobShopScheduling)

# 设置C++标准和相关属性
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_VISIBILITY_INLINES_HIDDEN ON)
set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)

# 包含子CMake脚本
include(cmake/directories.cmake)
include(cmake/sources.cmake)
include(cmake/options.cmake)

# 设置模块路径并查找pybind11
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
find_package(pybind11 REQUIRED)

add_library(jobshop_core STATIC ${SOURCES} ${HEADERS})
target_include_directories(jobshop_core
        PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)
target_link_options(jobshop_core PRIVATE -static-libgcc -static-libstdc++)
target_compile_definitions(jobshop_core
        PRIVATE
        JOB_SHOP_CORE_EXPORT
)

# 生成主可执行文件
add_executable(${CMAKE_PROJECT_NAME} main.cpp)
target_link_libraries(${CMAKE_PROJECT_NAME}
        PRIVATE
        jobshop_core
)

# 示例程序
add_executable(example example/example.cpp)
target_link_libraries(example
        PRIVATE
        jobshop_core
)

# Python绑定模块
add_subdirectory(src/python)