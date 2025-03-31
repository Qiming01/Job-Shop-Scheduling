cmake_minimum_required(VERSION 3.30)
project(JobShopScheduling)

set(CMAKE_CXX_STANDARD 20)

# 设置 include 目录
include_directories(${CMAKE_SOURCE_DIR}/include)

# 查找所有源文件
file(GLOB_RECURSE SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)

# 查找所有头文件
file(GLOB_RECURSE HEADERS ${CMAKE_SOURCE_DIR}/include/*.h ${CMAKE_SOURCE_DIR}/include/*.hpp)

# 生成可执行文件
add_executable(${CMAKE_PROJECT_NAME} ${SOURCES} ${HEADERS})