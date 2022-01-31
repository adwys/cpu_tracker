cmake_minimum_required(VERSION 3.16)
project(cpu_tracker C)

#set(CMAKE_C_COMPILER 99)
set(CMAKE_C_COMPILER /usr/bin/cc)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options("-Weverything")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "gcc")
    add_compile_options(
            "-Wextra"
            "-Wall"
    )
endif()

add_executable(cpu_tracker main.c cpuUsageTracker.c)
target_link_libraries(cpu_tracker pthread )