cmake_minimum_required(VERSION 3.30)
project(JobShopScheduling)

# 包含子CMake脚本
include(cmake/directories.cmake)
include(cmake/sources.cmake)
include(cmake/options.cmake)

# 设置C++标准
set(CMAKE_CXX_STANDARD 20)

# 生成可执行文件
add_executable(${CMAKE_PROJECT_NAME} main.cpp ${SOURCES} ${HEADERS})

# 示例程序
add_executable(example example/example.cpp ${SOURCES} ${HEADERS})