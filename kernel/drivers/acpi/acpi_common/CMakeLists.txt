cmake_minimum_required(VERSION 3.15)

file(GLOB_RECURSE C_SRCS "src/*.c")
add_library(driver_acpi_common OBJECT ${C_SRCS})
target_sources(
    driver_acpi_common PUBLIC
    $<TARGET_OBJECTS:libk>
)
target_link_libraries(driver_acpi_common PUBLIC libk)
target_include_directories(driver_acpi_common PUBLIC "include/")