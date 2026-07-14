cmake_minimum_required(VERSION 3.15)
enable_language(ASM-ATT)

file(GLOB_RECURSE C_SRCS "src/*.c")
file(GLOB_RECURSE ASM_SRCS "src/*.s")
add_library(bsp_machine_i386-pc OBJECT ${C_SRCS} ${ASM_SRCS})
target_include_directories(bsp_machine_i386-pc PUBLIC "include/")