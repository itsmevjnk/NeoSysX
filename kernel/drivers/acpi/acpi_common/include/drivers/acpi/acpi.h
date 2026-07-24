#ifndef __DRIVERS_ACPI_H
#define __DRIVERS_ACPI_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define ACPI_RSDP_SIGNATURE "RSD PTR "
typedef union {
    struct {
        union {
            struct {
                char signature[8];
                uint8_t checksum; // only applies for the RSDP section
                char oem_id[6];
                uint8_t revision;
                uint32_t rsdt_addr; // xsdt_addr should be used in ACPI 2.0+
            
            } __attribute__((packed));
            uint8_t rsdp_bytes[20];
        };

        // XSDP - only valid in ACPI 2.0+
        uint32_t len;
        uint64_t xsdt_addr;
        uint8_t ext_checksum; // applies for the entire XSDP
        
    } __attribute__((packed));
    uint8_t xsdp_bytes[0];
} __attribute__((packed)) acpi_rsdp_t;

bool acpi_verify_rsdp(const acpi_rsdp_t* rsdp); // verify RSDP validity - should be called by acpi_get_rsdp for verification
uintptr_t acpi_find_rsdp(uintptr_t from_vaddr, size_t len, size_t align); // find an RSDP in the specified range

// to be defined by ACPI implementation (ACPICA/uACPI/LAI)
bool acpi_init(void);

// to be defined by machine/target
uint64_t acpi_get_rsdp(void); // return RSDP physical address, or UINT64_MAX if there's none

#endif