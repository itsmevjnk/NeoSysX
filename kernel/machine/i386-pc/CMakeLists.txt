cmake_minimum_required(VERSION 3.15)
enable_language(ASM-ATT)

file(GLOB_RECURSE C_SRCS "src/*.c")
file(GLOB_RECURSE ASM_SRCS "src/*.s")
add_library(bsp_machine_i386-pc OBJECT ${C_SRCS} ${ASM_SRCS})
target_sources(
    bsp_machine_i386-pc PUBLIC
    $<TARGET_OBJECTS:bsp_cpu_i386>
    $<TARGET_OBJECTS:driver_uart_16550_pc>
    $<TARGET_OBJECTS:driver_video_vga_text>
    $<TARGET_OBJECTS:kernel_common>
    $<TARGET_OBJECTS:libk>
)
target_link_libraries(bsp_machine_i386-pc PUBLIC bsp_cpu_i386 driver_uart_16550_pc driver_video_vga_text kernel_common libk)
target_include_directories(bsp_machine_i386-pc PUBLIC "include/")