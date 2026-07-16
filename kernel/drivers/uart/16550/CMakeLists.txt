cmake_minimum_required(VERSION 3.15)

file(GLOB_RECURSE C_SRCS "src/*.c")
add_library(driver_uart_16550 OBJECT ${C_SRCS})
target_include_directories(driver_uart_16550 PUBLIC "include/")