cmake_minimum_required(VERSION 3.15)

file(GLOB_RECURSE C_SRCS "src/*.c")
add_library(kernel_common OBJECT ${C_SRCS})
target_sources(
    kernel_common PUBLIC
    $<TARGET_OBJECTS:libk>
)
target_link_libraries(kernel_common PUBLIC libk)
target_link_libraries(kernel_common PUBLIC gcc)
target_include_directories(kernel_common PUBLIC "include/")

add_library(kernel_common_headers INTERFACE)
target_include_directories(kernel_common_headers INTERFACE "include/")