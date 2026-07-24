#include <drivers/acpi/acpi.h>
#include <string.h>
#include <kernel/log.h>

bool acpi_verify_rsdp(const acpi_rsdp_t* rsdp) {
    if (memcmp(rsdp->signature, ACPI_RSDP_SIGNATURE, sizeof(rsdp->signature))) {
        LOG_ERROR("RSDP signature does not match (%.8s, expected " ACPI_RSDP_SIGNATURE ")", rsdp->signature);
        return false;
    }
    
    /* verify RSDP checksum */
    uint8_t sum = 0;
    for (size_t i = 0; i < sizeof(rsdp->rsdp_bytes); i++) sum += rsdp->rsdp_bytes[i];
    if (sum) {
        LOG_ERROR("RSDP checksum verification failed (%u)", sum);
        return false;
    }

    /* verify extended checksum */
    if (rsdp->revision > 1) {
        for (size_t i = sizeof(rsdp->rsdp_bytes); i < rsdp->len; i++) sum += rsdp->xsdp_bytes[i];
        if (sum) {
            LOG_ERROR("XSDP extended checksum verification failed (%u)", sum);
            return false;
        }
    }

    return true;
}

uintptr_t acpi_find_rsdp(uintptr_t from_vaddr, size_t len, size_t align) {
    if (align < 1) align = 1;
    size_t remainder = from_vaddr % align;
    if (remainder) {
        size_t delta = align - remainder;
        from_vaddr += delta;
        if (len > delta) len -= delta; else len = 0;
    }

    for (size_t offset = 0; offset < len; offset += align, from_vaddr += align) {
        const acpi_rsdp_t* rsdp = (const acpi_rsdp_t*)from_vaddr;
        if (memcmp(rsdp->signature, ACPI_RSDP_SIGNATURE, 8)) continue;

        if (!acpi_verify_rsdp(rsdp)) continue;
        return from_vaddr;
    }
    
    return UINTPTR_MAX;
}