# 设置宏参数
# add_compile_definitions(FIND_WITH_PRIORITY_QUEUE)
# add_compile_definitions(PRECISE_EVALUATE)
# add_compile_definitions(PRINT_INFO)

# 设置平台特定的编译和链接选项
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    # Windows 平台设置
    if (MINGW)
        # MinGW 编译器设置
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static -static-libgcc -static-libstdc++")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive")
    elseif (MSVC)
        # MSVC 编译器设置
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MT")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:MSVCRT")
    endif ()
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    # Linux 平台设置
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    # macOS 平台设置
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++")
endif ()