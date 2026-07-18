#include <mm/pmm.h>
#include <i386-pc-multiboot/multiboot.h>
#include <string.h>
#include <kernel/log.h>

uint64_t pmm_get_size(void) {
    uint64_t ret = 0;
    for (multiboot_memory_map_t* mmap = (multiboot_memory_map_t*)mb_info->mmap_addr; mmap; mmap = mb_traverse_mmap(mmap)) {
        if (mmap->type == MULTIBOOT_MEMORY_AVAILABLE) {
            uint64_t addr = ((uint64_t)mmap->addr_h << 32) | mmap->addr_l;
            uint64_t size = ((uint64_t)mmap->len_h << 32) | mmap->len_l;
            uint64_t end_addr = addr + size;
            if (end_addr > ret) ret = end_addr;
        }
    }
    return ret;
}

extern uintptr_t __kernel_end;

uintptr_t pmm_get_available_start(void) {
    uintptr_t ret = (uintptr_t)&__kernel_end;
    
    /* Multiboot info structure */
    uintptr_t mb_info_end = (uintptr_t)mb_info + sizeof(multiboot_info_t);
    if (mb_info_end > ret) ret = mb_info_end;

    /* command line */
    if ((mb_info->flags & MULTIBOOT_INFO_CMDLINE) && mb_info->cmdline) {
        const char* cmdline = (const char*)mb_info->cmdline;
        uintptr_t cmdline_end = mb_info->cmdline + strlen(cmdline) + 1;
        if (cmdline_end > ret) ret = cmdline_end;
    }

    /* boot modules */
    if ((mb_info->flags & MULTIBOOT_INFO_MODS) && mb_info->mods_addr && mb_info->mods_count) {
        uintptr_t mods_end = mb_info->mods_addr + mb_info->mods_count * sizeof(multiboot_module_t);
        if (mods_end > ret) ret = mods_end;

        const multiboot_module_t* module = (const multiboot_module_t*)mb_info->mods_addr;
        for (size_t i = 0; i < mb_info->mods_count; i++, module++) {
            if (module->mod_end > ret) ret = module->mod_end;
            if (module->cmdline) {
                const char* cmdline = (const char*)module->cmdline;
                uintptr_t cmdline_end = module->cmdline + strlen(cmdline) + 1;
                if (cmdline_end > ret) ret = cmdline_end;
            }
        }
    }

    /* memory map */
    if ((mb_info->flags & MULTIBOOT_INFO_MEM_MAP) && mb_info->mmap_addr && mb_info->mmap_length) {
        uintptr_t end = mb_info->mmap_addr + mb_info->mmap_length;
        if (end > ret) ret = end;
    }

    /* drive info */
    if ((mb_info->flags & MULTIBOOT_INFO_DRIVE_INFO) && mb_info->drives_addr && mb_info->drives_length) {
        uintptr_t end = mb_info->drives_addr + mb_info->drives_length;
        if (end > ret) ret = end;
    }

    return ret;
}

void pmm_target_init(void) {
    /* reserve BIOS-related memory regions in the first 1MB */
    pmm_reserve(0x00000, 0x004FF - 0x00000 + 1); // real mode IVT and BDA
    pmm_reserve(0x80000, 0x9FFFF - 0x80000 + 1); // EBDA
    pmm_reserve(0xA0000, 0xBFFFF - 0xA0000 + 1); // video memory
    pmm_reserve(0xC0000, 0xC7FFF - 0xC0000 + 1); // video BIOS
    pmm_reserve(0xC8000, 0xEFFFF - 0xC8000 + 1); // BIOS expansions
    pmm_reserve(0xF0000, 0xFFFFF - 0xF0000 + 1); // BIOS

    /* Multiboot info structure */
    LOG_DEBUG("reserving Multiboot info structure at 0x%lx (%lu bytes)", (uintptr_t)mb_info, sizeof(multiboot_info_t));
    pmm_reserve((uintptr_t)mb_info, sizeof(multiboot_info_t));

    /* command line */
    if ((mb_info->flags & MULTIBOOT_INFO_CMDLINE) && mb_info->cmdline) {
        const char* cmdline = (const char*)mb_info->cmdline;
        size_t len = strlen(cmdline) + 1;
        LOG_DEBUG("reserving Multiboot cmdline at 0x%lx (%lu bytes)", (uintptr_t)cmdline, len);
        pmm_reserve((uintptr_t)cmdline, len);
    }

    /* boot modules */
    if ((mb_info->flags & MULTIBOOT_INFO_MODS) && mb_info->mods_addr && mb_info->mods_count) {
        size_t len = mb_info->mods_count * sizeof(multiboot_module_t);
        LOG_DEBUG("reserving Multiboot modules table at 0x%x (%lu bytes)", mb_info->mods_addr, len);
        pmm_reserve(mb_info->mods_addr, len);

        const multiboot_module_t* module = (const multiboot_module_t*)mb_info->mods_addr;
        for (size_t i = 0; i < mb_info->mods_count; i++, module++) {
            len = module->mod_end - module->mod_start;
            LOG_DEBUG("reserving Multiboot module %lu at 0x%x (%lu bytes)", i, module->mod_start, len);
            pmm_reserve(module->mod_start, len);

            if (module->cmdline) {
                const char* cmdline = (const char*)module->cmdline;
                size_t len = strlen(cmdline) + 1;
                LOG_DEBUG("reserving Multiboot module %lu cmdline at 0x%lx (%lu bytes)", i, (uintptr_t)cmdline, len);
                pmm_reserve((uintptr_t)cmdline, len);
            }
        }
    }

    /* memory map */
    if ((mb_info->flags & MULTIBOOT_INFO_MEM_MAP) && mb_info->mmap_addr && mb_info->mmap_length) {
        LOG_DEBUG("reserving Multiboot memory map at 0x%x (%u bytes)", mb_info->mmap_addr, mb_info->mmap_length);
        pmm_reserve(mb_info->mmap_addr, mb_info->mmap_length);

        for (multiboot_memory_map_t* mmap = (multiboot_memory_map_t*)mb_info->mmap_addr; mmap; mmap = mb_traverse_mmap(mmap)) {
            if (mmap->type != MULTIBOOT_MEMORY_AVAILABLE) {
                uint64_t addr = ((uint64_t)mmap->addr_h << 32) | mmap->addr_l;
                uint64_t size = ((uint64_t)mmap->len_h << 32) | mmap->len_l;
                LOG_DEBUG("reserving memory region at 0x%llx (%llu bytes)", addr, size);
                pmm_reserve(addr, size);
            }
        }
    }

    /* drive info */
    if ((mb_info->flags & MULTIBOOT_INFO_DRIVE_INFO) && mb_info->drives_addr && mb_info->drives_length) {
        LOG_DEBUG("reserving Multiboot drives table at 0x%x (%u bytes)", mb_info->drives_addr, mb_info->drives_length);
        pmm_reserve(mb_info->drives_addr, mb_info->drives_length);
    }
}
