cmake_minimum_required(VERSION 3.30)
project(JobShopScheduling)

set(CMAKE_CXX_STANDARD 20)

# 设置 include 目录
include_directories(${CMAKE_SOURCE_DIR}/include)

# 查找所有源文件
file(GLOB_RECURSE SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)

# 查找所有头文件
file(GLOB_RECURSE HEADERS ${CMAKE_SOURCE_DIR}/include/*.h ${CMAKE_SOURCE_DIR}/include/*.hpp)


# 设置 可执行程序输出目录
set(publish_bin_debug 	${CMAKE_CURRENT_SOURCE_DIR}/bin/debug)
set(publish_bin_release ${CMAKE_CURRENT_SOURCE_DIR}/bin/release)

# 指定可执行程序输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG 	${publish_bin_debug})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE 	${publish_bin_release})

# 生成可执行文件
add_executable(${CMAKE_PROJECT_NAME} ${SOURCES} ${HEADERS})