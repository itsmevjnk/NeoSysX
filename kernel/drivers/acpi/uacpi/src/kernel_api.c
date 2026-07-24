#include <uacpi/kernel_api.h>
#include <mm/vmm.h>
#include <mm/pmm.h>
#include <drivers/acpi/acpi.h>
#include <kernel/log.h>

void *uacpi_kernel_map(uacpi_phys_addr addr, uacpi_size len) {
    uintptr_t vaddr = vmm_map_paddr_kernel_addrspace(addr, len, VMM_FLAG_RW | VMM_FLAG_CACHE_GLOBAL);
    if (vaddr == UINTPTR_MAX) return UACPI_MAP_FAILED;

    pmm_reserve(addr, len);
    return (void*)vaddr;
}

void uacpi_kernel_unmap(void *addr, uacpi_size len) {
    uint64_t paddr;
    if (!vmm_get_paddr_kernel((uintptr_t)addr, &paddr)) return; // not mapped yet

    pmm_free(paddr, len);
    vmm_unmap_kernel((uintptr_t)addr, len);
}

void uacpi_kernel_log(uacpi_log_level level, const uacpi_char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    const char* header;
    switch (level) {
        case UACPI_LOG_DEBUG:
            header = LOG_DEBUG_HEADER;
            break;
        case UACPI_LOG_TRACE:
            header = LOG_DEBUG_HEADER;
            break;
        case UACPI_LOG_INFO:
            header = LOG_INFO_HEADER;
            break;
        case UACPI_LOG_WARN:
            header = LOG_WARN_HEADER;
            break;
        case UACPI_LOG_ERROR:
            header = LOG_ERROR_HEADER;
            break;
        default:
            header = " ";
            break;
    }
    uint64_t __ts = timer_get();
    printf("%s %llu.%06u\x1B[0m ", header, __ts / TIMER_TICK_1_SEC, (unsigned)(__ts % TIMER_TICK_1_SEC) / 1000);
    vprintf(fmt, args);
    va_end(args);
}

uacpi_status uacpi_kernel_get_rsdp(uacpi_phys_addr *out_rsdp_address) {
    uint64_t addr = acpi_get_rsdp();
    if (addr == UINT64_MAX) return UACPI_STATUS_NOT_FOUND;
    *out_rsdp_address = addr;
    return UACPI_STATUS_OK;
}
