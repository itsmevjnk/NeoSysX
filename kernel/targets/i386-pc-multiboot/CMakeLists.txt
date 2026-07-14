cmake_minimum_required(VERSION 3.15)
enable_language(ASM-ATT)

file(GLOB_RECURSE C_SRCS "src/*.c")
file(GLOB_RECURSE ASM_SRCS "src/*.s")
add_executable(target_i386-pc-multiboot ${C_SRCS} ${ASM_SRCS})
target_sources(
    target_i386-pc-multiboot PRIVATE
    $<TARGET_OBJECTS:kernel_common>
    $<TARGET_OBJECTS:bsp_cpu_i386>
)
target_include_directories(target_i386-pc-multiboot PUBLIC "include/")
set_target_properties(
    target_i386-pc-multiboot PROPERTIES 
    LINK_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/link.ld"
)
set_target_properties(
    target_i386-pc-multiboot PROPERTIES 
    OUTPUT_NAME "kernel.elf"
)
target_link_options(
    target_i386-pc-multiboot PRIVATE
    "-T${CMAKE_CURRENT_LIST_DIR}/link.ld"
)