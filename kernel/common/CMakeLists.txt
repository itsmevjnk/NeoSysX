cmake_minimum_required(VERSION 3.15)

set(CPU_ARCH_HEADERS "$<TARGET_NAME_IF_EXISTS:bsp_cpu_${NEOSYSX_CPU}_arch_headers>")

file(GLOB_RECURSE C_SRCS "src/*.c")
add_library(kernel_common OBJECT ${C_SRCS})
target_sources(
    kernel_common PUBLIC
    $<TARGET_OBJECTS:libk>
)
target_link_libraries(kernel_common PUBLIC libk)
target_link_libraries(kernel_common PUBLIC gcc)
target_link_libraries(kernel_common PUBLIC ${CPU_ARCH_HEADERS})
target_include_directories(kernel_common PUBLIC "include/")

add_library(kernel_common_headers INTERFACE)
target_link_libraries(kernel_common_headers INTERFACE ${CPU_ARCH_HEADERS})
target_include_directories(kernel_common_headers INTERFACE "include/")