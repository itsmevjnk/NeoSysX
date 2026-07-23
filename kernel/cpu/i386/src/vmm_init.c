#include <cpu/i386/vmm.h>
#include <mm/vmm.h>

extern uintptr_t __kernel_lh_phys_start;
extern uintptr_t __kernel_lh_start;
extern uintptr_t __kernel_lh_phys_end;
extern uintptr_t __kernel_lh_end;
extern uintptr_t __text_phys_start;
extern uintptr_t __text_start;
extern uintptr_t __text_phys_end;
extern uintptr_t __text_end;
extern uintptr_t __rodata_phys_start;
extern uintptr_t __rodata_start;
extern uintptr_t __rodata_phys_end;
extern uintptr_t __rodata_end;

__attribute__((weak)) void vmm_target_init(void) {
    
}

void vmm_init(void) {
    vmm_target_init();

    /* unmap identity mapping */
    for (size_t pde = 0; pde < (0xC0000000 >> 22); pde++) {
        if (!vmm_kernel_config.pd[pde].val) break;
        vmm_kernel_config.pd[pde].val = 0;
        vmm_kernel_config.pt[pde] = NULL;
        for (size_t pte = 0; pte < vmm_kernel_config.pt_used[pde]; pte++) {
            vmm_invalidate_page((pde << 22) | (pte << 12)); // since pages are globally mapped, we have to use invlpg
        }
        vmm_kernel_config.pt_used[pde] = 0;
    }

    /* lock down read-only sections */
    vmm_map_kernel((uintptr_t)&__kernel_lh_phys_start, (uintptr_t)&__kernel_lh_start, (uintptr_t)&__kernel_lh_end - (uintptr_t)&__kernel_lh_start, VMM_FLAG_CACHE_GLOBAL);
    vmm_map_kernel((uintptr_t)&__text_phys_start, (uintptr_t)&__text_start, (uintptr_t)&__text_end - (uintptr_t)&__text_start, VMM_FLAG_CACHE_GLOBAL);
    vmm_map_kernel((uintptr_t)&__rodata_phys_start, (uintptr_t)&__rodata_start, (uintptr_t)&__rodata_end - (uintptr_t)&__rodata_start, VMM_FLAG_CACHE_GLOBAL);
}

