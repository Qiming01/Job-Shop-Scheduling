# 设置 include 目录
include_directories(${CMAKE_SOURCE_DIR}/include)

# 设置可执行程序输出目录
set(publish_bin_debug ${CMAKE_CURRENT_SOURCE_DIR}/bin/debug)
set(publish_bin_release ${CMAKE_CURRENT_SOURCE_DIR}/bin/release)

# 指定可执行程序输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${publish_bin_debug})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${publish_bin_release})