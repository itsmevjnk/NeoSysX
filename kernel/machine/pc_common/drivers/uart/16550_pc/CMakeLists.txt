cmake_minimum_required(VERSION 3.15)

file(GLOB_RECURSE C_SRCS "src/*.c")
add_library(driver_uart_16550_pc OBJECT ${C_SRCS})
target_sources(
    driver_uart_16550_pc PUBLIC
    $<TARGET_OBJECTS:driver_uart_16550>
    $<TARGET_OBJECTS:driver_uart_16550_term>
    $<TARGET_OBJECTS:kernel_common>
)
target_link_libraries(
    driver_uart_16550_pc PUBLIC
    driver_uart_16550
    driver_uart_16550_term
    bsp_cpu_x86_common
    kernel_common
)
target_include_directories(driver_uart_16550_pc PUBLIC "include/")