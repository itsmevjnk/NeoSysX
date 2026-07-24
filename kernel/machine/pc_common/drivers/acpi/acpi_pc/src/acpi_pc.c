#include <drivers/acpi/acpi_pc.h>
#include <drivers/acpi/acpi.h>
#include <mm/vmm.h>

uint64_t acpi_pc_get_rsdp(void) {
    /* check EBDA */
    // map BDA to extract EBDA pointer
    uintptr_t bda_ebda_ptr = vmm_map_paddr_kernel_addrspace(0x40E, 2, 0); // read-only, no need for caching here
    if (bda_ebda_ptr == UINTPTR_MAX) return UINT64_MAX; // cannot map EBDA pointer
    uintptr_t ebda_paddr = *((uint16_t*)bda_ebda_ptr) << 4;
    vmm_unmap_kernel(bda_ebda_ptr, 2);

    // map EBDA
    uintptr_t ebda_vaddr = vmm_map_paddr_kernel_addrspace(ebda_paddr, 1024, 0);
    if (ebda_vaddr == UINTPTR_MAX) return UINT64_MAX; // cannot map EBDA
    
    // search within unmapped EBDA, then clean up
    uint64_t rsdp_paddr = UINT64_MAX;
    uintptr_t rsdp_vaddr = acpi_find_rsdp(ebda_vaddr, 1024, 16); // RSDP signature is always on a 16-byte boundary
    if (rsdp_vaddr != UINTPTR_MAX) {
        vmm_get_paddr_kernel(rsdp_vaddr, &rsdp_paddr);
    }
    vmm_unmap_kernel(ebda_vaddr, 1024);
    if (rsdp_paddr != UINT64_MAX) return rsdp_paddr;

    /* check BIOS region (0xE0000 - 0xFFFFF) */
    uintptr_t bios_vaddr = vmm_map_paddr_kernel_addrspace(0xE0000, 0x20000, 0);
    if (bios_vaddr == UINTPTR_MAX) return UINT64_MAX; // cannot map BIOS region
    rsdp_vaddr = acpi_find_rsdp(bios_vaddr, 0x20000, 16);
    if (rsdp_vaddr != UINTPTR_MAX) {
        vmm_get_paddr_kernel(rsdp_vaddr, &rsdp_paddr);
    }
    vmm_unmap_kernel(bios_vaddr, 0x20000);
    return rsdp_paddr;
}

// NOTE: this can be overridden by the target if there's a quicker method available; the target can use acpi_pc_get_rsdp as a fallback
__attribute__((weak)) uint64_t acpi_get_rsdp(void) {
    return acpi_pc_get_rsdp();
}