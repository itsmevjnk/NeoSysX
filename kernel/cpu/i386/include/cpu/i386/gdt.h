#ifndef __CPU_I386_GDT_H
#define __CPU_I386_GDT_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint16_t limit_lo : 16;
    uint32_t base_lo : 24;

    union {
        struct {
            uint8_t accessed : 1; // should be set to 1
            uint8_t rw : 1;
            uint8_t dc : 1;
            uint8_t code : 1;
            uint8_t desc_type : 1;
            uint8_t dpl : 2;
            uint8_t present : 1;
        } __attribute__((packed)) cseg_dseg;
        struct {
            uint8_t type : 4;
            uint8_t desc_type : 1;
            uint8_t dpl : 2;
            uint8_t present : 1;
        } __attribute__((packed)) sysseg;
        uint8_t byte;
    } access;

    uint8_t limit_hi : 4;

    uint8_t reserved : 1;
    uint8_t long_code : 1;
    uint8_t size : 1;
    uint8_t granularity : 1;

    uint8_t base_hi : 8;
} __attribute__((packed)) gdt_entry_t;

typedef struct {
    uint16_t size;
    gdt_entry_t* offset;
} __attribute__((packed)) gdt_descriptor_t;

#define GDT_NULL (gdt_entry_t){0}
#define GDT_SEGMENT(base, limit, rw_bit, dc_bit, code_bit, type_bit, dpl_val, is_pm32, is_4k) (gdt_entry_t){ \
    .limit_lo = ((limit) & 0xFFFF), \
    .base_lo = ((base) & 0xFFFFFF), \
    .access = { \
        .cseg_dseg = { \
            .accessed = 1, \
            .rw = (rw_bit), \
            .dc = (dc_bit), \
            .code = (code_bit), \
            .desc_type = (type_bit), \
            .dpl = (dpl_val), \
            .present = 1, \
        } \
    }, \
    .limit_hi = (((limit) >> 16) & 0xF), \
    .reserved = 0, \
    .long_code = 0, \
    .size = (is_pm32), \
    .granularity = (is_4k), \
    .base_hi = (((base) >> 24) & 0xFF) \
}
#define GDT_BYTE_SEGMENT(base, size, rw_bit, dc_bit, code_bit, type_bit, dpl_val, is_pm32) GDT_SEGMENT(base, ((size) - 1), rw_bit, dc_bit, code_bit, type_bit, dpl_val, is_pm32, 0)
#define GDT_4K_SEGMENT(base, size, rw_bit, dc_bit, code_bit, type_bit, dpl_val, is_pm32) GDT_SEGMENT(base, (((size) - 1) >> 12), rw_bit, dc_bit, code_bit, type_bit, dpl_val, is_pm32, 1)
#define GDT_BYTE_CSEG16(base, size, conforming, dpl_val) GDT_BYTE_SEGMENT(base, size, 1, conforming, 1, 1, dpl_val, 0)
#define GDT_BYTE_CSEG32(base, size, conforming, dpl_val) GDT_BYTE_SEGMENT(base, size, 1, conforming, 1, 1, dpl_val, 1)
#define GDT_4K_CSEG16(base, size, conforming, dpl_val) GDT_4K_SEGMENT(base, size, 1, conforming, 1, 1, dpl_val, 0)
#define GDT_4K_CSEG32(base, size, conforming, dpl_val) GDT_4K_SEGMENT(base, size, 1, conforming, 1, 1, dpl_val, 1)
#define GDT_BYTE_DSEG16(base, size, dpl_val) GDT_BYTE_SEGMENT(base, size, 1, 0, 0, 1, dpl_val, 0)
#define GDT_BYTE_DSEG32(base, size, dpl_val) GDT_BYTE_SEGMENT(base, size, 1, 0, 0, 1, dpl_val, 1)
#define GDT_4K_DSEG16(base, size, dpl_val) GDT_4K_SEGMENT(base, size, 1, 0, 0, 1, dpl_val, 0)
#define GDT_4K_DSEG32(base, size, dpl_val) GDT_4K_SEGMENT(base, size, 1, 0, 0, 1, dpl_val, 1)

void gdt_init(void); // NOTE: implemented in gdt.s

#endif /* __CPU_I386_GDT_H */