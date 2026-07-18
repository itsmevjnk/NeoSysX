cmake_minimum_required(VERSION 3.15)

file(GLOB_RECURSE C_SRCS "src/*.c")
add_library(driver_intr_i8259 OBJECT ${C_SRCS})
target_sources(
    driver_intr_i8259 PUBLIC
    $<TARGET_OBJECTS:kernel_common>
)
target_link_libraries(
    driver_intr_i8259 PUBLIC
    bsp_cpu_x86_common
    bsp_cpu_i386
    kernel_common
)
target_include_directories(driver_intr_i8259 PUBLIC "include/")