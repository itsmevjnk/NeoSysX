#include <cpu/i386/vmm.h>
#include <cpu/i386/pmm.h>
#include <mm/pmm.h>

__attribute__((weak)) void kinit_target_lh(void) {
    
}

__attribute__((section(".text.lh")))
void vmm_lh_init(void) {
    pmm_initialise_available_start();

    uintptr_t pt_paddr = *((uintptr_t*)((uintptr_t)&pmm_available_start - 0xC0000000)) + pmm_get_bitmap_size_lh(); // physical address of next page table to be allocated
    vmm_config_t* vmm_info = (vmm_config_t*)((uintptr_t)&vmm_kernel_config - 0xC0000000);

    /* map physical address to both lower and higher halves, we'll unmap the lower half once we're in higher half */
    uintptr_t paddr = 0; // physical address of next page to be mapped
    while (paddr < pt_paddr) { // play catch up with page tables that we're allocating
        size_t pde_id = paddr >> 22, pte = (paddr >> 12) & 0x3FF;
        size_t pde_hh = pde_id + (0xC0000000 >> 22); // higher half PDE
        if (!pte && paddr + 0x400000 <= pt_paddr) { // use large pages
            vmm_info->pd[pde_id].large.addr = paddr >> 22;
            vmm_info->pd[pde_id].large.present = 1;
            vmm_info->pd[pde_id].large.rw = 1;
            vmm_info->pd[pde_id].large.size = 1;
            vmm_info->pd[pde_id].large.global = 1;
            vmm_info->pd[pde_hh].val = vmm_info->pd[pde_id].val;
            vmm_info->pt_used[pde_id] = vmm_info->pt_used[pde_hh] = 1024;
            paddr += 0x400000;
        } else { // use small pages
            if (!vmm_info->pt[pde_id]) { // allocate page table
                vmm_info->pt[pde_id] = vmm_info->pt[pde_hh] = (vmm_pt_entry_t*)(pt_paddr + 0xC0000000);
                vmm_info->pd[pde_id].small.pt_paddr = pt_paddr >> 12;
                vmm_info->pd[pde_id].small.present = 1;
                vmm_info->pd[pde_id].small.rw = 1;
                vmm_info->pd[pde_id].small.size = 0;
                vmm_info->pd[pde_hh].val = vmm_info->pd[pde_id].val;

                // clear out this memory chunk
                uint8_t* pt_buf = (uint8_t*)pt_paddr;
                for (size_t i = 0; i < 4096; i++) {
                    pt_buf[i] = 0;
                }

                pt_paddr += 4096;
            }
            vmm_pt_entry_t* pt = (vmm_pt_entry_t*)((uintptr_t)vmm_info->pt[pde_id] - 0xC0000000);
            pt[pte].addr = paddr >> 12;
            pt[pte].present = 1;
            pt[pte].rw = 1;
            pt[pte].global = 1;
            vmm_info->pt_used[pde_id]++;
            vmm_info->pt_used[pde_hh]++;
            paddr += 0x1000;
        }
    }

    vmm_info->cr3 = (uintptr_t)&vmm_kernel_config.pd - 0xC0000000; // populate CR3
}

void vmm_reserve_initial_pt(void) {
    for (size_t pde = (0xC0000000 >> 22); pde < 1024; pde++) {
        if (vmm_kernel_config.pt[pde])
            pmm_reserve((uintptr_t)vmm_kernel_config.pt[pde] - 0xC0000000, 4096);
    }
}