#include <i386-pc-multiboot/multiboot.h>
#include <i386-pc-multiboot/multiboot_spec.h> // include the official Multiboot header file

/* Multiboot header */
#define MULTIBOOT_FLAGS                             (MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO)
__attribute__((section(".multiboot")))
struct multiboot_header multiboot_header = {
    .magic = MULTIBOOT_HEADER_MAGIC,
    .flags = MULTIBOOT_FLAGS,
    .checksum = -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_FLAGS)
};
