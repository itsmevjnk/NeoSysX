cmake_minimum_required(VERSION 3.15)

file(GLOB_RECURSE C_SRCS "src/*.c")
add_library(driver_acpi_pc OBJECT ${C_SRCS})
target_sources(
    driver_acpi_pc PUBLIC
    $<TARGET_OBJECTS:driver_acpi_common>
)
target_link_libraries(
    driver_acpi_pc PUBLIC
    driver_acpi_common
)
target_include_directories(driver_acpi_pc PUBLIC "include/")