cmake_minimum_required(VERSION 3.15)

file(GLOB_RECURSE C_SRCS "src/*.c")
add_library(libk OBJECT ${C_SRCS})
target_include_directories(libk PUBLIC "include/")