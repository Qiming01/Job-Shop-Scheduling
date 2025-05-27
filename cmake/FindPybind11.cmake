# 尝试通过常规方式查找 pybind11
find_package(pybind11 QUIET CONFIG)

if (NOT pybind11_FOUND)
    # 如果常规查找失败，尝试通过 pip 获取路径
    execute_process(
            COMMAND pip show pybind11
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
        # 完全找不到时的备选方案
        include(FetchContent)
        FetchContent_Declare(
                pybind11
                GIT_REPOSITORY https://github.com/pybind/pybind11.git
                GIT_TAG v2.11.1
        )
        FetchContent_MakeAvailable(pybind11)
        message(STATUS "Falling back to downloaded pybind11")
    endif ()
endif ()

# 验证查找结果
if (NOT pybind11_FOUND)
    message(FATAL_ERROR "pybind11 not found and automatic download failed")
endif ()