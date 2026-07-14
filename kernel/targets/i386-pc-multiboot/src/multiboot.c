#include <multiboot.h>
#include <multiboot2.h> // include the official Multiboot2 header file

/* Multiboot 2 header */
#define MULTIBOOT_FLAGS                             (MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO)
__attribute__((section(".multiboot")))
struct multiboot_header multiboot_header = {
    .magic = MULTIBOOT2_HEADER_MAGIC,
    .flags = MULTIBOOT_FLAGS,
    .checksum = -(MULTIBOOT2_HEADER_MAGIC + MULTIBOOT_FLAGS)
};
