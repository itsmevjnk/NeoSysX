#ifndef __CPU_I386_VMM_H
#define __CPU_I386_VMM_H

#include <stddef.h>
#include <stdint.h>

typedef union {
    struct {
        uint8_t present : 1;
        uint8_t rw : 1;
        uint8_t user : 1;
        uint8_t writethru : 1;
        uint8_t nocache : 1;
        uint8_t accessed : 1;
        uint8_t dirty : 1;
        uint8_t size : 1; // set to 1
        uint8_t global : 1;
        uint8_t available : 3;
        uint8_t attrib_tab : 1; // set to 0
        uint32_t addr_hi : 8; // bits 32-39
        uint8_t reserved : 1; // set to 0
        uint32_t addr : 10; // bits 22-31
    } __attribute__((packed)) large; // large (4M) pages
    struct {
        uint8_t present : 1;
        uint8_t rw : 1;
        uint8_t user : 1;
        uint8_t writethru : 1;
        uint8_t nocache : 1;
        uint8_t accessed : 1;
        uint8_t dirty : 1;
        uint8_t size : 1; // set to 0
        uint8_t available : 4;
        uint32_t pt_paddr : 20; // bits 12-31
    } __attribute__((packed)) small; // small pages - point to page table
    uint32_t val;
} __attribute__((packed)) vmm_pd_entry_t;

typedef struct {
    uint8_t present : 1;
    uint8_t rw : 1;
    uint8_t user : 1;
    uint8_t writethru : 1;
    uint8_t nocache : 1;
    uint8_t accessed : 1;
    uint8_t dirty : 1;
    uint8_t attrib_tab : 1;
    uint8_t global : 1;
    uint8_t available : 3;
    uint32_t addr : 20;
} __attribute__((packed)) vmm_pt_entry_t;


typedef struct {
    vmm_pd_entry_t pd[1024]; // page directory
    vmm_pt_entry_t* pt[1024]; // corresponding page tables
    uint32_t cr3; // page directory physical address
} __attribute__((packed)) vmm_info_t;

void vmm_reserve_initial_pt(void);

void kinit_target_lh(void); // to be implemented by target - called by _start_lh

#endif /* __CPU_I386_VMM_H */