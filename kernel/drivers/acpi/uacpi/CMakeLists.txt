cmake_minimum_required(VERSION 3.15)

include(FetchContent)
FetchContent_Declare(uacpi_src
    GIT_REPOSITORY https://github.com/uACPI/uACPI.git
    GIT_TAG 6.0.0
)
FetchContent_GetProperties(uacpi_src)
if(NOT uacpi_src_POPULATED)
    FetchContent_Populate(uacpi_src)
endif()

include("${uacpi_src_SOURCE_DIR}/uacpi.cmake")

add_library(uacpi_impl OBJECT ${UACPI_SOURCES})
target_compile_definitions(
    uacpi_impl PRIVATE
    UACPI_OVERRIDE_LIBC
    UACPI_BAREBONES_MODE # TODO: remove this once we have more kernel features (e.g. multitasking)
    UACPI_FORMATTED_LOGGING
)
target_sources(
    uacpi_impl PUBLIC
    $<TARGET_OBJECTS:libk>
    $<TARGET_OBJECTS:driver_acpi_common>
)
target_link_libraries(uacpi_impl PUBLIC libk driver_acpi_common)
target_include_directories(uacpi_impl PUBLIC ${UACPI_INCLUDES} "include/")

# disable -Werror from uacpi_impl compile options
set_target_properties(uacpi_impl PROPERTIES COMPILE_OPTIONS "-Wno-error")

file(GLOB_RECURSE C_SRCS "src/*.c")
add_library(driver_acpi_uacpi OBJECT ${C_SRCS})
target_sources(
    driver_acpi_uacpi PUBLIC
    $<TARGET_OBJECTS:uacpi_impl>
)
target_compile_definitions(
    driver_acpi_uacpi PRIVATE
    UACPI_FORMATTED_LOGGING
)
target_link_libraries(driver_acpi_uacpi PUBLIC uacpi_impl kernel_common_headers)
target_include_directories(driver_acpi_uacpi PUBLIC "include/")
