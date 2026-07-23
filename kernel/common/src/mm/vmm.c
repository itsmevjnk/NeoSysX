#include <mm/vmm.h>
#include <mm/pmm.h>

size_t vmm_map_current(uint64_t paddr, uintptr_t vaddr, size_t size, size_t flags) {
    return vmm_map(vmm_get_current_config(), paddr, vaddr, size, flags);
}

size_t vmm_map_kernel(uint64_t paddr, uintptr_t vaddr, size_t size, size_t flags) {
    return vmm_map(vmm_get_kernel_config(), paddr, vaddr, size, flags);
}

void vmm_unmap_current(uintptr_t vaddr, size_t size) {
    vmm_unmap(vmm_get_current_config(), vaddr, size);
}

void vmm_unmap_kernel(uintptr_t vaddr, size_t size) {
    vmm_unmap(vmm_get_kernel_config(), vaddr, size);
}

bool vmm_get_paddr_current(uintptr_t vaddr, uint64_t* paddr_out) {
    return vmm_get_paddr(vmm_get_current_config(), vaddr, paddr_out);
}

bool vmm_get_paddr_kernel(uintptr_t vaddr, uint64_t* paddr_out) {
    return vmm_get_paddr(vmm_get_kernel_config(), vaddr, paddr_out);
}

bool vmm_is_vaddr_used_current(uintptr_t vaddr) {
    return vmm_is_vaddr_used(vmm_get_current_config(), vaddr);
}

bool vmm_is_vaddr_used_kernel(uintptr_t vaddr) {
    return vmm_is_vaddr_used(vmm_get_kernel_config(), vaddr);
}

uintptr_t vmm_find_free_current(uintptr_t from_vaddr, size_t size) {
    return vmm_find_free(vmm_get_current_config(), from_vaddr, size);
}

uintptr_t vmm_find_free_kernel(uintptr_t from_vaddr, size_t size) {
    return vmm_find_free(vmm_get_kernel_config(), from_vaddr, size);
}

uintptr_t vmm_alloc(void* config, size_t size, size_t flags, uintptr_t from_vaddr) {
    uintptr_t vaddr = vmm_find_free(config, from_vaddr, size);
    if (vaddr == UINTPTR_MAX) return 0;
    
    if (flags & VMM_FLAG_ALLOC_CONTIGUOUS) {
        uint64_t paddr = pmm_alloc(size);
        if (paddr == UINT64_MAX) return 0;

        vmm_map(config, paddr, vaddr, size, flags & ~VMM_FLAG_ALLOC_CONTIGUOUS);
    } else {
        size_t frame_size = pmm_get_frame_size();
        size_t allocated_size = 0, remaining_size = size;
        size_t next_vaddr = vaddr;
        while (allocated_size < size) {
            size_t iter_size = remaining_size;
            uint64_t paddr = UINT64_MAX;
            while (1) {
                paddr = pmm_alloc(iter_size);
                if (paddr == UINT64_MAX) {
                    if (iter_size <= frame_size) break;
                    iter_size -= frame_size;
                    continue;
                } else break; 
            }
            if (paddr == UINT64_MAX) {
                /* clean up and return 0 */
                vmm_free_pmm_frames(config, vaddr, next_vaddr - vaddr);
                vmm_unmap(config, vaddr, next_vaddr - vaddr);
                return 0;
            }   
            allocated_size += iter_size;
            vmm_map(config, paddr, next_vaddr, frame_size, flags & ~VMM_FLAG_ALLOC_CONTIGUOUS);
            next_vaddr += iter_size;    
            if (remaining_size <= iter_size) break;
            remaining_size -= iter_size;
        }
    }

    return vaddr;
}

uintptr_t vmm_alloc_current(size_t size, size_t flags, uintptr_t from_vaddr) {
    return vmm_alloc(vmm_get_current_config(), size, flags, from_vaddr);
}

uintptr_t vmm_alloc_kernel(size_t size, size_t flags, uintptr_t from_vaddr) {
    return vmm_alloc(vmm_get_kernel_config(), size, flags, from_vaddr);
}

void vmm_free_pmm_frames(void* config, uintptr_t vaddr, size_t size) {
    size_t frame_size = pmm_get_frame_size();
    for (size_t freed_size = 0; freed_size < size; freed_size += frame_size, vaddr += frame_size) {
        uint64_t paddr;
        if (!vmm_get_paddr(config, vaddr, &paddr)) continue;
        pmm_free(paddr, frame_size);
    }
}

void vmm_free_pmm_frames_current(uintptr_t vaddr, size_t size) {
    vmm_free_pmm_frames(vmm_get_current_config(), vaddr, size);
}

void vmm_free_pmm_frames_kernel(uintptr_t vaddr, size_t size) {
    vmm_free_pmm_frames(vmm_get_kernel_config(), vaddr, size);
}

/* default implementation for vmm_find_free using primitives */
__attribute__((weak))
uintptr_t vmm_find_free(void* config, uintptr_t from_vaddr, size_t size) {
    size_t frame_size = pmm_get_frame_size(); // assumed to be page size
    uintptr_t vaddr = from_vaddr;
    if (vaddr % frame_size) vaddr += frame_size - vaddr % frame_size;
    size_t frames = (size + frame_size - 1) / frame_size;
    
    size_t free_frames = 0;
    uintptr_t iter_vaddr = vaddr;
    while (true) {
        if (!vmm_is_vaddr_used(config, iter_vaddr)) {
            free_frames++;
            if (free_frames == frames) return vaddr;
        } else free_frames = 0;
        if (iter_vaddr >= UINTPTR_MAX - frame_size + 1) return UINTPTR_MAX; // reached end of vaddr space
        else iter_vaddr += frame_size;
    }
}
