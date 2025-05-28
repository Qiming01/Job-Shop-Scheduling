# 设置 include 目录
include_directories(${CMAKE_SOURCE_DIR}/include)

# 设置可执行程序输出目录
set(publish_bin_debug ${CMAKE_CURRENT_SOURCE_DIR}/bin/debug)
set(publish_bin_release ${CMAKE_CURRENT_SOURCE_DIR}/bin/release)

# 指定可执行程序输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${publish_bin_debug})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${publish_bin_release})

# 如果外部没有传入 CMAKE_LIBRARY_OUTPUT_DIRECTORY，则设置默认值
if (NOT DEFINED CMAKE_LIBRARY_OUTPUT_DIRECTORY)
    set(PYTHON_MODULE_OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/bin/python_module")
else ()
    set(PYTHON_MODULE_OUTPUT_DIR "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
endif ()

message(STATUS "Python module output directory: ${PYTHON_MODULE_OUTPUT_DIR}")