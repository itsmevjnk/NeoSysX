cmake_minimum_required(VERSION 3.15)

file(GLOB_RECURSE C_SRCS "src/*.c")
add_library(driver_uart_16550_term OBJECT ${C_SRCS})
target_sources(
    driver_uart_16550_term PUBLIC
    $<TARGET_OBJECTS:kernel_common>
    $<TARGET_OBJECTS:driver_uart_16550>
)
target_link_libraries(driver_uart_16550_term PUBLIC kernel_common driver_uart_16550)
target_include_directories(driver_uart_16550_term PUBLIC "include/")