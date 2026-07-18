include("${CMAKE_CURRENT_LIST_DIR}/common.cmake")

set(CMAKE_C_COMPILER i686-elf-gcc) 
set(CMAKE_CXX_COMPILER i686-elf-g++)
set(CMAKE_ASM-ATT_COMPILER i686-elf-gcc)

# set PMM frame size
add_compile_definitions("PMM_FRAME_SIZE=4096")
