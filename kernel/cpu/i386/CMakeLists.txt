cmake_minimum_required(VERSION 3.15)
enable_language(ASM-ATT)

file(GLOB_RECURSE C_SRCS "src/*.c")
file(GLOB_RECURSE ASM_SRCS "src/*.s")
add_library(bsp_cpu_i386 OBJECT ${C_SRCS} ${ASM_SRCS})
target_sources(
    bsp_cpu_i386 PUBLIC
    $<TARGET_OBJECTS:libk>
)
target_link_libraries(bsp_cpu_i386 kernel_common_headers libk bsp_cpu_x86_common_arch_headers)
target_include_directories(bsp_cpu_i386 PUBLIC "include/")

add_library(bsp_cpu_i386_arch_headers INTERFACE)
target_link_libraries(bsp_cpu_i386_arch_headers INTERFACE bsp_cpu_x86_common_arch_headers)
