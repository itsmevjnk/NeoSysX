#ifndef __MM_PMM_H
#define __MM_PMM_H

#include <stddef.h>
#include <stdint.h>

void pmm_init(void);
size_t pmm_get_frame_size(void);

void pmm_reserve(uint64_t addr, uint64_t len);
uint64_t pmm_alloc(uint64_t len); // return UINTPTR_MAX if allocation fails
void pmm_free(uint64_t addr, uint64_t len);

/* to be implemented by target */
uint64_t pmm_get_size(void); // get the size of the RAM-mapped physical address space, including any reserved regions within
uintptr_t pmm_get_available_start(void); // get the pointer to the start of available physical memory (outside of kernel and bootloader-provided information structures)
void pmm_target_init(void); // use pmm_reserve() to reserve memory sections as needed

#endif