cmake_minimum_required(VERSION 3.15)
enable_language(ASM-ATT)

file(GLOB_RECURSE C_SRCS "src/*.c")
file(GLOB_RECURSE ASM_SRCS "src/*.s")
add_library(bsp_cpu_x86_common OBJECT ${C_SRCS} ${ASM_SRCS})
target_link_libraries(bsp_cpu_x86_common kernel_common_headers)
target_include_directories(bsp_cpu_x86_common PUBLIC "include/")
