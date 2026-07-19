#ifndef __CPU_I386_PMM_H
#define __CPU_I386_PMM_H

#include <stddef.h>
#include <stdint.h>

extern uintptr_t pmm_available_start; // NOTE: this is in higher half!

size_t pmm_get_bitmap_size_lh(void); // get PMM bitmap size (only accessible in lower half)

void pmm_cpu_init(void);

/* to be implemented by target */
void pmm_initialise_available_start(void); // must be executed and located in lower half
size_t pmm_get_size_lh(void); // duplicate of pmm_get_size that must be executable in lower half

#endif