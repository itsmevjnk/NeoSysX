#include <cpu/i386/vmm.h>
#include <i386-pc-multiboot/multiboot.h>

/* relocate Multiboot pointers to higher half */
static void vmm_relocate_multiboot(void) {    
    /* command line */
    if ((mb_info->flags & MULTIBOOT_INFO_CMDLINE) && mb_info->cmdline) {
        mb_info->cmdline += 0xC0000000;
    }

    /* boot modules */
    if ((mb_info->flags & MULTIBOOT_INFO_MODS) && mb_info->mods_addr) {
        mb_info->mods_addr += 0xC0000000;

        multiboot_module_t* module = (multiboot_module_t*)mb_info->mods_addr;
        for (size_t i = 0; i < mb_info->mods_count; i++, module++) {
            module->mod_start += 0xC0000000;
            module->mod_end += 0xC0000000;
            if (module->cmdline) module->cmdline += 0xC0000000;
        }
    }

    /* memory map */
    if ((mb_info->flags & MULTIBOOT_INFO_MEM_MAP) && mb_info->mmap_addr) {
        mb_info->mmap_addr += 0xC0000000;
    }

    /* drive info */
    if ((mb_info->flags & MULTIBOOT_INFO_DRIVE_INFO) && mb_info->drives_addr) {
        mb_info->drives_addr += 0xC0000000;
    }
}

void vmm_target_init(void) {
    vmm_relocate_multiboot();
}