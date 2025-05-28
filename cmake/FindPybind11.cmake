if (POLICY CMP0148)
    cmake_policy(SET CMP0148 NEW)
endif ()

find_package(Python COMPONENTS Interpreter Development REQUIRED)

# 尝试通过常规方式查找 pybind11
find_package(pybind11 QUIET CONFIG)

if (NOT pybind11_FOUND)
    # 如果常规查找失败，尝试通过 pip 获取路径
    execute_process(
            COMMAND ${Python_EXECUTABLE} -m pip show pybind11
            OUTPUT_VARIABLE PYTHON_PYBIND11_INFO
            RESULT_VARIABLE PIP_RESULT
            ERROR_QUIET
    )

    if (PIP_RESULT EQUAL 0)
        # 解析 pip show 的输出
        string(REGEX MATCH "Location: ([^\n]+)" _ ${PYTHON_PYBIND11_INFO})
        set(PYBIND11_ROOT_DIR ${CMAKE_MATCH_1})

        # 设置 pybind11_DIR 为 pip 安装路径下的 share/cmake/pybind11
        set(pybind11_DIR "${PYBIND11_ROOT_DIR}/pybind11/share/cmake/pybind11")
        message(STATUS "Found pybind11 via pip: ${pybind11_DIR}")

        # 再次尝试查找
        find_package(pybind11 CONFIG REQUIRED)
    else ()
        message(FATAL_ERROR "pybind11 not found. Please install pybind11 via pip:\n"
                "  ${Python_EXECUTABLE} -m pip install pybind11")
    endif ()
endif ()

# 验证查找结果
if (NOT pybind11_FOUND)
    message(FATAL_ERROR "pybind11 not found. Tried:\n"
            "1. System installation\n"
            "2. pip installation\n"
            "Please install pybind11 using one of these methods.")
endif ()