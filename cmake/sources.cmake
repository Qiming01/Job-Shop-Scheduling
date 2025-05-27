# 查找所有源文件
file(GLOB SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)

# 查找所有头文件
file(GLOB_RECURSE HEADERS ${CMAKE_SOURCE_DIR}/include/*.h ${CMAKE_SOURCE_DIR}/include/*.hpp)