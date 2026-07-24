#include <drivers/acpi/acpi.h>
#include <uacpi/uacpi.h>
#include <mm/vmm.h>
#include <kernel/log.h>

static void* acpi_temp_table = NULL;

#ifndef ACPI_TEMP_TABLE_SIZE
#define ACPI_TEMP_TABLE_SIZE 8192
#endif

bool acpi_init(void) {
    uintptr_t temp_table = vmm_alloc_kernel_addrspace(ACPI_TEMP_TABLE_SIZE, VMM_FLAG_RW | VMM_FLAG_CACHE_GLOBAL);
    if (temp_table == UINTPTR_MAX) {
        LOG_ERROR("unable to allocate temporary table for uACPI");
        return false;
    }
    acpi_temp_table = (void*)temp_table;
    uacpi_setup_early_table_access(acpi_temp_table, ACPI_TEMP_TABLE_SIZE);

    return true;
}
