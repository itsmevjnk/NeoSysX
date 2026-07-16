cmake_minimum_required(VERSION 3.15)

file(GLOB_RECURSE C_SRCS "src/*.c")
add_library(driver_video_vga_text OBJECT ${C_SRCS})
target_sources(
    driver_video_vga_text PUBLIC
    $<TARGET_OBJECTS:kernel_common>
)
target_link_libraries(
    driver_video_vga_text PUBLIC
    bsp_cpu_x86_common
    kernel_common
)
target_include_directories(driver_video_vga_text PUBLIC "include/")