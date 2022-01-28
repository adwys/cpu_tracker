cmake_minimum_required(VERSION 3.16)
project(cpu_usage_tracker C)

#set(CMAKE_C_COMPILER /usr/bin/clang)
set(CMAKE_C_STANDARD 99)

add_compile_options(
        "-Wextra"
        "-Wall"
#        "-Weverything"
)

add_executable(cpu_usage_tracker main.c cpuUsageTracker.c)
target_link_libraries(cpu_usage_tracker pthread )
