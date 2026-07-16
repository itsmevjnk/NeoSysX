cmake_minimum_required(VERSION 3.15)
enable_language(ASM-ATT)

add_library(bsp_cpu_x86_common INTERFACE)
target_include_directories(bsp_cpu_x86_common INTERFACE "include/")