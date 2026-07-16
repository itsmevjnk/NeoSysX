cmake_minimum_required(VERSION 3.15)

include(FetchContent)
FetchContent_Declare(printf_library
    GIT_REPOSITORY https://github.com/eyalroz/printf.git
    GIT_TAG v6.3.0
)
FetchContent_GetProperties(printf_library)
if(NOT printf_library_POPULATED)
    FetchContent_Populate(printf_library)
endif()

add_library(printf OBJECT "${printf_library_SOURCE_DIR}/src/printf/printf.c")
target_compile_definitions(
    printf PRIVATE
    PRINTF_ALIAS_STANDARD_FUNCTION_NAMES_HARD=1
    PRINTF_INCLUDE_CONFIG_H=1
)
target_include_directories(printf PRIVATE "${printf_library_SOURCE_DIR}/src/" "include/")

file(GLOB_RECURSE C_SRCS "src/*.c")
add_library(libk OBJECT ${C_SRCS})
target_sources(
    libk PUBLIC
    $<TARGET_OBJECTS:printf>
)
target_link_libraries(libk PUBLIC kernel_common_headers)
target_include_directories(libk PUBLIC "include/")