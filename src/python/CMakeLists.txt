pybind11_add_module(jsp MODULE bindings.cpp)
target_link_libraries(jsp PRIVATE pybind11::module jobshop_core)
target_include_directories(jsp PRIVATE ${CMAKE_SOURCE_DIR}/include)

# 单独设置Python模块的输出目录
set_target_properties(jsp PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${PYTHON_MODULE_OUTPUT_DIR}
        RUNTIME_OUTPUT_DIRECTORY ${PYTHON_MODULE_OUTPUT_DIR}
)