cmake_minimum_required(VERSION 3.15)

file(GLOB_RECURSE C_SRCS "src/*.c")
add_library(kernel_common OBJECT ${C_SRCS})
target_sources(
    kernel_common PRIVATE
    $<TARGET_OBJECTS:libk>
)
target_include_directories(kernel_common PUBLIC "include/")