cmake_minimum_required(VERSION 3.15)
enable_language(ASM-ATT)

file(GLOB_RECURSE C_SRCS "src/*.c")
file(GLOB_RECURSE ASM_SRCS "src/*.s")
add_executable(i386-pc-multiboot_kernel ${C_SRCS} ${ASM_SRCS})
target_sources(
    i386-pc-multiboot_kernel PUBLIC
    $<TARGET_OBJECTS:kernel_common>
    $<TARGET_OBJECTS:bsp_machine_i386-pc>
)
target_link_libraries(i386-pc-multiboot_kernel PUBLIC kernel_common bsp_machine_i386-pc)
target_include_directories(i386-pc-multiboot_kernel PUBLIC "include/")
set_target_properties(
    i386-pc-multiboot_kernel PROPERTIES 
    LINK_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/link.ld"
)
set_target_properties(
    i386-pc-multiboot_kernel PROPERTIES 
    OUTPUT_NAME "kernel.elf"
)
target_link_options(
    i386-pc-multiboot_kernel PRIVATE
    "-T${CMAKE_CURRENT_LIST_DIR}/link.ld"
)
add_custom_command(
    TARGET i386-pc-multiboot_kernel
    POST_BUILD
    COMMAND grub-file --is-x86-multiboot $<TARGET_FILE:i386-pc-multiboot_kernel>
    COMMENT "Verifying resulting kernel is Multiboot-compliant"
)