#ifndef __MM_VMM_H
#define __MM_VMM_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define VMM_FLAG_RW                             (1 << 0)
#define VMM_FLAG_USER                           (1 << 1)
#define VMM_FLAG_CACHE_GLOBAL                   (1 << 2)
#define VMM_FLAG_ALLOC_CONTIGUOUS               (1 << 31) // only applicable in vmm_alloc* - physical address space must be contiguous

size_t vmm_map_current(uint64_t paddr, uintptr_t vaddr, size_t size, size_t flags);
size_t vmm_map_kernel(uint64_t paddr, uintptr_t vaddr, size_t size, size_t flags);

void vmm_unmap_current(uintptr_t vaddr, size_t size);
void vmm_unmap_kernel(uintptr_t vaddr, size_t size);

bool vmm_get_paddr_current(uintptr_t vaddr, uint64_t* paddr_out);
bool vmm_get_paddr_kernel(uintptr_t vaddr, uint64_t* paddr_out);

bool vmm_is_vaddr_used_current(uintptr_t vaddr);
bool vmm_is_vaddr_used_kernel(uintptr_t vaddr);

uintptr_t vmm_find_free_current(uintptr_t from_vaddr, size_t size);
uintptr_t vmm_find_free_kernel(uintptr_t from_vaddr, size_t size);

uintptr_t vmm_map_paddr(void* config, uintptr_t paddr, uintptr_t from_vaddr, size_t size, size_t flags);
uintptr_t vmm_map_paddr_current(uintptr_t paddr, uintptr_t from_vaddr, size_t size, size_t flags);
uintptr_t vmm_map_paddr_kernel(uintptr_t paddr, uintptr_t from_vaddr, size_t size, size_t flags);
uintptr_t vmm_map_paddr_kernel_addrspace(uintptr_t paddr, size_t size, size_t flags);

uintptr_t vmm_alloc(void* config, size_t size, size_t flags, uintptr_t from_vaddr);
uintptr_t vmm_alloc_current(size_t size, size_t flags, uintptr_t from_vaddr);
uintptr_t vmm_alloc_kernel(size_t size, size_t flags, uintptr_t from_vaddr);
uintptr_t vmm_alloc_kernel_addrspace(size_t size, size_t flags);

void vmm_free_pmm_frames(void* config, uintptr_t vaddr, size_t size);
void vmm_free_pmm_frames_current(uintptr_t vaddr, size_t size);
void vmm_free_pmm_frames_kernel(uintptr_t vaddr, size_t size);

/* to be implemented by CPU BSP */
void vmm_init(void);
void* vmm_get_current_config(void); // return pointer to the current VMM configuration
void* vmm_get_kernel_config(void); // return pointer to the kernel's initial VMM configuration
void* vmm_new_config(void* source); // allocate and initialise a new VMM config, optionally copying from a non-NULL config (otherwise it will copy the kernel config)
void vmm_delete_config(void* config); // deallocate VMM config
size_t vmm_map(void* config, uint64_t paddr, uintptr_t vaddr, size_t size, size_t flags);
void vmm_unmap(void* config, uintptr_t vaddr, size_t size);
// NOTE: the BSP VMM implementation must ensure that all maps/unmaps made in the kernel memory region must be propagated to all VMM configurations it manages
bool vmm_is_vaddr_used(void* config, uintptr_t vaddr); // check if the specified virtual address is being mapped/reserved
uintptr_t vmm_find_free(void* config, uintptr_t from_vaddr, size_t size); // find the first contiguous chunk of available virtual address space with size, starting from the specified vaddr
// NOTE: vmm_find_free is optional, though it is highly recommended if the CPU BSP can provide an efficient implementation of this
bool vmm_get_paddr(void* config, uintptr_t vaddr, uint64_t* paddr_out); // get the physical address mapped to the specified virtual address (returns false if vaddr is unmapped)
void vmm_switch(void* config); // switch to another VMM config

#endif