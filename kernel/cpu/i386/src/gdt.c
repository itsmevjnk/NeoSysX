#include <cpu/i386/gdt.h>

gdt_entry_t gdt_table[5] = {
    GDT_NULL,
    GDT_4K_CSEG32(0, 0x100000000ULL, 1, 0), // 0x08 - ring 0 code segment
    GDT_4K_DSEG32(0, 0x100000000ULL, 0), // 0x10 - ring 0 data segment
    GDT_4K_CSEG32(0, 0x100000000ULL, 0, 3), // 0x18 - ring 3 data segment
    GDT_4K_DSEG32(0, 0x100000000ULL, 3), // 0x20 - ring 3 data segment
};

gdt_descriptor_t gdt_desc = {
    .size = sizeof(gdt_table),
    .offset = gdt_table
};
