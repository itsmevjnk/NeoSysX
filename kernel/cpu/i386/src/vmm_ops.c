#include <cpu/i386/vmm.h>
#include <mm/vmm.h>
#include <mm/pmm.h>
#include <string.h>
#include <cpu/intr.h>

__attribute__((aligned(4096)))
vmm_config_t vmm_kernel_config = {
    .pd = {{.val = 0}}
};

vmm_config_t* vmm_current_config = &vmm_kernel_config;

void* vmm_get_current_config(void) {
    return (void*)vmm_current_config;
}

void* vmm_get_kernel_config(void) {
    return (void*)&vmm_kernel_config;
}

#define VMM_PD_ENTRIES 1024
#define VMM_PD_SIZE (VMM_PD_ENTRIES * sizeof(vmm_pd_entry_t))
#define VMM_PT_ENTRIES 1024
#define VMM_PT_SIZE (VMM_PT_ENTRIES * sizeof(vmm_pt_entry_t))

#define KERNEL_PDE_START                        (0xC0000000 >> 22)

void* vmm_new_config(void* source) {
    vmm_config_t* config = (vmm_config_t*)vmm_alloc_kernel_addrspace(sizeof(vmm_config_t), VMM_FLAG_RW | VMM_FLAG_CACHE_GLOBAL | VMM_FLAG_ALLOC_CONTIGUOUS);
    if (!config) return NULL;
    uint64_t cr3 = ~0;
    bool paddr_success = vmm_get_paddr_kernel((uintptr_t)&config->pd, &cr3);
    if (!paddr_success || cr3 > UINT32_MAX) {
        vmm_unmap_kernel((uintptr_t)config, sizeof(vmm_config_t));
        pmm_free(cr3, sizeof(vmm_config_t));
        return NULL;
    }
    config->cr3 = cr3;
    config->next = vmm_kernel_config.next;
    vmm_kernel_config.next = config;
    
    memset((void*)config, 0, sizeof(vmm_config_t));

    /* clone kernel address space */
    memcpy(&config->pd[KERNEL_PDE_START], &vmm_kernel_config.pd[KERNEL_PDE_START], (VMM_PD_ENTRIES - KERNEL_PDE_START) * sizeof(vmm_pd_entry_t));
    memcpy(&config->pt[KERNEL_PDE_START], &vmm_kernel_config.pt[KERNEL_PDE_START], (VMM_PD_ENTRIES - KERNEL_PDE_START) * sizeof(vmm_pt_entry_t*));
    memcpy(&config->pt_used[KERNEL_PDE_START], &vmm_kernel_config.pt_used[KERNEL_PDE_START], (VMM_PD_ENTRIES - KERNEL_PDE_START) * sizeof(uint16_t));

    /* clone user address space */
    if (source) {
        vmm_config_t* src_config = (vmm_config_t*)source;
        for (size_t pde = 0; pde < KERNEL_PDE_START; pde++) {
            if (!src_config->pd[pde].val) continue;
            config->pd[pde].val = src_config->pd[pde].val; // copy all PDE fields over first
            if (!config->pd[pde].large.size) { // small page - clone the corresponding PT
                config->pt[pde] = (vmm_pt_entry_t*)vmm_alloc_kernel_addrspace(VMM_PT_ENTRIES * sizeof(vmm_pt_entry_t), VMM_FLAG_RW | VMM_FLAG_CACHE_GLOBAL | VMM_FLAG_ALLOC_CONTIGUOUS);
                bool success = config->pt[pde] != NULL;
                if (success) { // allocated - now get the physical address and insert into the PDE
                    uint64_t paddr = ~0;
                    paddr_success = vmm_get_paddr_kernel((uintptr_t)config->pt[pde], &paddr);
                    if (paddr_success || paddr > UINT32_MAX) {
                        success = false;
                    } else {
                        config->pd[pde].small.pt_paddr = paddr >> 12;
                    }
                }
                if (!success) { // allocation failure
                    for (size_t i = 0; i <= pde; i++) {
                        if (!config->pt[i]) continue;
                        vmm_unmap_kernel((uintptr_t)config->pt[i], VMM_PT_ENTRIES * sizeof(vmm_pt_entry_t));
                        pmm_free(config->pd[i].small.pt_paddr << 12, VMM_PT_ENTRIES * sizeof(vmm_pt_entry_t));
                    }

                    vmm_unmap_kernel((uintptr_t)config, sizeof(vmm_config_t));
                    pmm_free(cr3, sizeof(vmm_config_t));
                    return NULL;
                }
            }
        }
        memcpy(&config->pt_used, &src_config->pt_used, KERNEL_PDE_START * sizeof(uint16_t));
    }

    return (void*)config;
}

void vmm_delete_config(void* config) {
    uint64_t paddr;
    if (!vmm_get_paddr_kernel((uintptr_t)config, &paddr)) return;

    vmm_free_pmm_frames_kernel((uintptr_t)config, sizeof(vmm_config_t));
    vmm_unmap_kernel((uintptr_t)config, sizeof(vmm_config_t));
    pmm_free(paddr, sizeof(vmm_config_t));
}

static inline void vmm_propagate_pde(vmm_config_t* config, size_t pde) {
    vmm_config_t* dest = &vmm_kernel_config;
    for (; dest; dest = dest->next) {
        if (dest != config) {
            dest->pt[pde] = config->pt[pde];
            dest->pd[pde].val = config->pd[pde].val;
            dest->pt_used[pde] = config->pt_used[pde];
        }
    }
}

size_t vmm_map(void* config, uint64_t paddr, uintptr_t vaddr, size_t size, size_t flags) {
    if ((paddr & 0xFFF) || (vaddr & 0xFFF)) {
        size_t paddr_off = paddr & 0xFFF, vaddr_off = vaddr & 0xFFF;
        paddr &= ~0xFFF; vaddr &= ~0xFFF;
        size += 0x1000 - ((paddr_off < vaddr_off) ? paddr_off : vaddr_off);
    }

    size_t mapped_size = 0;
    vmm_config_t* cfg = (vmm_config_t*) config;
    if (!cfg) return 0;
    for (; mapped_size < size; mapped_size += 0x1000, paddr += 0x1000, vaddr += 0x1000) {
        size_t pde = vaddr >> 22, pte = (vaddr >> 12) & 0x3FF;
        bool large_eligible = !pte && (mapped_size - size >= 0x400000); // whether this region can be mapped as a large page
        
        if (large_eligible) { // map as 4M page
            if (cfg->pt[pde]) { // there's already a table here
                vmm_unmap_kernel((uintptr_t)cfg->pt[pde], VMM_PT_SIZE);
                cfg->pt[pde] = NULL;
            }

            cfg->pd[pde].large.addr = paddr >> 22;
            cfg->pd[pde].large.present = 1;
            cfg->pd[pde].large.rw = (flags & VMM_FLAG_RW) ? 1 : 0;
            cfg->pd[pde].large.user = (flags & VMM_FLAG_USER) ? 1 : 0;
            cfg->pd[pde].large.global = (flags & VMM_FLAG_CACHE_GLOBAL) ? 1 : 0;
            cfg->pt_used[pde] = VMM_PT_ENTRIES; // all pages mapped

            if (pde >= KERNEL_PDE_START) vmm_propagate_pde(cfg, pde);

            if (cfg == vmm_current_config || (flags & VMM_FLAG_CACHE_GLOBAL)) {
                for (size_t i = 0; i < VMM_PT_ENTRIES; i++)
                    vmm_invalidate_page((pde << 22) | (i << 12));
            }
            
            paddr += 0x400000 - 0x1000;
            vaddr += 0x400000 - 0x1000;
            size += 0x400000 - 0x1000; 
            // NOTE: we will add 0x1000 back in as part of the for loop
        } else {
            bool propagate = false;
            if (!cfg->pt[pde]) { // no page table here
                cfg->pt[pde] = (vmm_pt_entry_t*)vmm_alloc_kernel_addrspace(VMM_PT_SIZE, VMM_FLAG_RW | VMM_FLAG_CACHE_GLOBAL);
                if (!cfg->pt[pde]) break;

                uint64_t pt_paddr;
                vmm_get_paddr(config, (uintptr_t)cfg->pt[pde], &pt_paddr);
                if (pt_paddr > UINT32_MAX) { // page table cannot be in the > 4G space
                    vmm_unmap_kernel((uintptr_t)cfg->pt[pde], VMM_PT_SIZE);
                    cfg->pt[pde] = NULL;
                    break;
                }

                memset(cfg->pt[pde], 0, VMM_PT_SIZE);

                if (cfg->pd[pde].large.size) { // 4M mapped page - remap to PT first
                    uint32_t frame = cfg->pd[pde].large.addr << (22 - 12);
                    for (size_t i = 0; i < VMM_PT_ENTRIES; i++, frame++) {
                        cfg->pt[pde][i].addr = frame;
                        cfg->pt[pde][i].present = cfg->pd[pde].large.present;
                        cfg->pt[pde][i].rw = cfg->pd[pde].large.rw;
                        cfg->pt[pde][i].user = cfg->pd[pde].large.user;
                        cfg->pt[pde][i].writethru = cfg->pd[pde].large.writethru;
                        cfg->pt[pde][i].nocache = cfg->pd[pde].large.nocache;
                        cfg->pt[pde][i].accessed = cfg->pd[pde].large.accessed;
                        cfg->pt[pde][i].dirty = cfg->pd[pde].large.dirty;
                        cfg->pt[pde][i].global = cfg->pd[pde].large.global;
                        cfg->pt[pde][i].attrib_tab = cfg->pd[pde].large.attrib_tab;
                    }
                    cfg->pt_used[pde] = VMM_PT_ENTRIES;
                } else cfg->pt_used[pde] = 0;

                cfg->pd[pde].val = 0;
                cfg->pd[pde].small.pt_paddr = (uint32_t)pt_paddr >> 12;
                cfg->pd[pde].small.present = 1;
                cfg->pd[pde].small.rw = 1;
                cfg->pd[pde].small.user = 1; // whether this page is actually user is controlled by the PT

                propagate = true;
            }

            if (!*((uint32_t*)&cfg->pt[pde][pte]) && cfg->pt_used[pde] < VMM_PT_ENTRIES) {
                cfg->pt_used[pde]++; // increment for unmapped page
                propagate = true;
            }

            cfg->pt[pde][pte].addr = paddr >> 12;
            cfg->pt[pde][pte].present = 1;
            cfg->pt[pde][pte].rw = (flags & VMM_FLAG_RW) ? 1 : 0;
            cfg->pt[pde][pte].user = (flags & VMM_FLAG_USER) ? 1 : 0;
            cfg->pt[pde][pte].global = (flags & VMM_FLAG_CACHE_GLOBAL) ? 1 : 0;

            if (propagate && pde >= KERNEL_PDE_START)
                vmm_propagate_pde(cfg, pde);

            if (cfg == vmm_current_config || (flags & VMM_FLAG_CACHE_GLOBAL)) vmm_invalidate_page(vaddr);
        }
    }

    return mapped_size;
}

void vmm_unmap(void* config, uintptr_t vaddr, size_t size) {
    vmm_config_t* cfg = (vmm_config_t*) config;
    if (!cfg) return;

    if (vaddr & 0xFFF) {
        size += vaddr & 0xFFF;
        vaddr &= ~0xFFF;
    }

    size_t unmapped_size = 0;
    size_t vaddr_end = vaddr + size;
    while (unmapped_size < size) {
        size_t pde = vaddr >> 22, pte = (vaddr >> 12) & 0x3FF;

        if (!cfg->pd[pde].val) { // entire PDE is not mapped
            uintptr_t next_vaddr = ((pde + 1) << 22); // address of beginning of next PDE
            unmapped_size += next_vaddr - vaddr;
            vaddr = next_vaddr;
            continue;
        }

        if (cfg->pd[pde].large.size) { // large page in use here
            uintptr_t next_vaddr = ((pde + 1) << 22);
            if (!pte && next_vaddr <= vaddr_end) { // the entire page can be unmapped
                cfg->pd[pde].val = 0;
                cfg->pt_used[pde] = 0;
                // NOTE: cfg->pt[pde] should already be null here
            } else { // convert to small pages
                cfg->pt[pde] = (vmm_pt_entry_t*)vmm_alloc_kernel_addrspace(VMM_PT_SIZE, VMM_FLAG_RW | VMM_FLAG_CACHE_GLOBAL);
                if (!cfg->pt[pde]) break; // TODO

                uint64_t pt_paddr;
                vmm_get_paddr(config, (uintptr_t)cfg->pt[pde], &pt_paddr);
                if (pt_paddr > UINT32_MAX) { // page table cannot be in the > 4G space
                    vmm_unmap_kernel((uintptr_t)cfg->pt[pde], VMM_PT_SIZE);
                    cfg->pt[pde] = NULL;
                    // TODO
                    break;
                }

                memset(cfg->pt[pde], 0, VMM_PT_SIZE);

                uint32_t frame = cfg->pd[pde].large.addr << (22 - 12);
                size_t from_pte = pte, to_pte = (((vaddr_end - 1) >> 22) != pde) ? (VMM_PT_ENTRIES - 1) : ((vaddr_end - 1) >> 22) & 0x3FF;
                for (size_t i = 0; i < VMM_PT_ENTRIES; i++, frame++) {
                    if (i >= from_pte && i <= to_pte) {
                        if (cfg == vmm_current_config || cfg->pd[pde].large.global) {
                            vmm_invalidate_page((pde << 22) | (i << 12)); // invalidate pages we're unmapping
                        }
                    } else {
                        cfg->pt[pde][i].addr = frame;
                        cfg->pt[pde][i].present = cfg->pd[pde].large.present;
                        cfg->pt[pde][i].rw = cfg->pd[pde].large.rw;
                        cfg->pt[pde][i].user = cfg->pd[pde].large.user;
                        cfg->pt[pde][i].writethru = cfg->pd[pde].large.writethru;
                        cfg->pt[pde][i].nocache = cfg->pd[pde].large.nocache;
                        cfg->pt[pde][i].accessed = cfg->pd[pde].large.accessed;
                        cfg->pt[pde][i].dirty = cfg->pd[pde].large.dirty;
                        cfg->pt[pde][i].global = cfg->pd[pde].large.global;
                        cfg->pt[pde][i].attrib_tab = cfg->pd[pde].large.attrib_tab;
                    }
                }

                cfg->pd[pde].val = 0;
                cfg->pd[pde].small.pt_paddr = (uint32_t)pt_paddr >> 12;
                cfg->pd[pde].small.present = 1;
                cfg->pd[pde].small.rw = 1;
                cfg->pd[pde].small.user = 1; // whether this page is actually user is controlled by the PT

                cfg->pt_used[pde] = 1024 - (to_pte - from_pte + 1);
            }
                
            unmapped_size += next_vaddr - vaddr;
            vaddr = next_vaddr;
            if (pde >= KERNEL_PDE_START) vmm_propagate_pde(cfg, pde); // propagate kernel pages
        } else { // small pages in use here
            // NOTE: cfg->pt[pde] should be non-null here
            uint8_t global = cfg->pt[pde][pte].global;
            *((uint32_t*)&cfg->pt[pde][pte]) = 0;
            if (cfg == vmm_current_config || global)
                vmm_invalidate_page(vaddr); // invalidate TLB cache for this address
            
            bool propagate = false;
            if (cfg->pt_used[pde]) {
                cfg->pt_used[pde]--;
                propagate = true;
            }
            if (!cfg->pt_used[pde]) { // no more pages - this PT can go
                vmm_unmap_kernel((uintptr_t)cfg->pt[pde], sizeof(vmm_pt_entry_t) * VMM_PT_ENTRIES);
                uintptr_t pt_paddr = cfg->pd[pde].small.pt_paddr << 12;
                pmm_free(pt_paddr, sizeof(vmm_pt_entry_t) * VMM_PT_ENTRIES);
                cfg->pd[pde].val = 0; cfg->pt[pde] = NULL;
                propagate = true;
            }

            unmapped_size += 0x1000;
            vaddr += 0x1000;
            if (propagate && pde >= KERNEL_PDE_START)
                vmm_propagate_pde(cfg, pde);
        }
    }
}

bool vmm_is_vaddr_used(void* config, uintptr_t vaddr) {
    const vmm_config_t* cfg = (vmm_config_t*)config;
    if (!cfg) return false;

    size_t pde = vaddr >> 22, pte = (vaddr >> 12) & 0x3FF;
    if (!cfg->pd[pde].val) return false;
    if (cfg->pd[pde].large.size) return true; // mapped as large page
    else return (*((uint32_t*)&cfg->pt[pde][pte]) != 0);
}

uintptr_t vmm_find_free(void* config, uintptr_t from_vaddr, size_t size) {
    const vmm_config_t* cfg = (vmm_config_t*)config;
    if (!cfg) return UINTPTR_MAX;

    if (from_vaddr & 0xFFF) from_vaddr += 0x1000 - (from_vaddr & 0xFFF);
    size_t pde = from_vaddr >> 22, pte = (from_vaddr >> 12) & 0x3FF;
    size_t pages = (size + 4095) >> 12;
    if (!pages) return UINTPTR_MAX;

    while (pde < VMM_PD_ENTRIES && pte < VMM_PT_ENTRIES) {
        if (cfg->pt_used[pde] >= VMM_PT_ENTRIES) { // PT is fully occupied
            pde++; pte = 0; // go to beginning of next PT
            continue;
        }

        if (!cfg->pt_used[pde] && pte + pages < VMM_PT_ENTRIES) { // entire PT is empty, and the requested pages can fit in the PT
            return (pde << 22) | (pte << 12);
        }
        
        /* look for continuous gap */
        size_t avail_pages = 0;
        uintptr_t ret = UINTPTR_MAX;
        while (avail_pages < pages && pde < VMM_PD_ENTRIES && pte < VMM_PT_ENTRIES) {
            if (!cfg->pt_used[pde]) { // entire PT is not used
                avail_pages += VMM_PT_ENTRIES - pte;
                if (!~ret) ret = (pde << 22) | (pte << 12);
                pde++; pte = 0;
            } else { // PT is partially used - must be small page mapped
                if (!*((uint32_t*)&cfg->pt[pde][pte])) { // PTE is empty
                    avail_pages++;
                    if (!~ret) ret = (pde << 22) | (pte << 12);
                } else {
                    if (~ret) break; // broken chain
                    pte++;
                    if (pte >= VMM_PT_ENTRIES) {
                        pte = 0;
                        pde++;
                    }
                }
            }
        }
        if (avail_pages < pages) continue; // find next gap
        return ret;
    }

    return UINTPTR_MAX;
}

bool vmm_get_paddr(void* config, uintptr_t vaddr, uint64_t* paddr_out) {
    const vmm_config_t* cfg = (vmm_config_t*)config;
    if (!cfg) return false;

    size_t pde = vaddr >> 22, pte = (vaddr >> 12) & 0x3FF;
    if (!cfg->pd[pde].val) return false; // not mapped
    if (cfg->pd[pde].large.size) {  // mapped as large page
        if (paddr_out) *paddr_out = ((uint64_t)cfg->pd[pde].large.addr_hi << 32) | (cfg->pd[pde].large.addr << 22) | (vaddr & 0x3FFFFF);
        return true;
    } else if (!*((uint32_t*)&cfg->pt[pde][pte])) return false; // not mapped
    else { // mapped as small page
        if (paddr_out) *paddr_out = (cfg->pt[pde][pte].addr << 12) | (vaddr & 0xFFF);
        return true;
    }
}

void vmm_switch(void* config) {
    if (!config) return;
    bool intr = intr_enabled();
    __asm__ volatile("cli"); // ensure atomicity of setting vmm_current_config
    __asm__ volatile("mov %0, %%cr3" : : "r"(config) : "memory");
    vmm_current_config = (vmm_config_t*)config;
    if (intr) __asm__ volatile("sti"); // re-enable interrupt if needed
}