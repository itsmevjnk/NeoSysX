#include <mm/pmm.h>
#include <machine/i386-pc/pmm.h>
#include <cpu/i386/pmm.h>
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

__attribute__((section(".text.lh")))
uint64_t pmm_get_size_lh(void) {
    uint64_t ret = 0;
    multiboot_info_t* mb_info_ptr_phys = *((multiboot_info_t**)((uintptr_t)&mb_info - 0xC0000000));
    multiboot_info_t* mb_info_phys = (multiboot_info_t*)((uintptr_t)mb_info_ptr_phys - 0xC0000000);
    multiboot_memory_map_t* mmap = (multiboot_memory_map_t*)mb_info_phys->mmap_addr;
    while (1) {
        if (mmap->type == MULTIBOOT_MEMORY_AVAILABLE) {
            uint64_t addr = ((uint64_t)mmap->addr_h << 32) | mmap->addr_l;
            uint64_t size = ((uint64_t)mmap->len_h << 32) | mmap->len_l;
            uint64_t end_addr = addr + size;
            if (end_addr > ret) ret = end_addr;
        }

        mmap = (multiboot_memory_map_t*)((uintptr_t)mmap + sizeof(mmap->size) + mmap->size);
        if ((uintptr_t)mmap >= mb_info_phys->mmap_addr + mb_info_phys->mmap_length) break;
    }
    return ret;
}

extern uintptr_t __kernel_phys_end;

__attribute__((section(".text.lh")))
size_t strlen_lh(const char* str) {
    // NOTE: we can instead call memchr(str, 0, SIZE_MAX)
    size_t len = 0;
    for (; str[len]; len++);
    return len;
}

__attribute__((section(".text.lh")))
void pmm_initialise_available_start(void) {
    uintptr_t* avail_start = (uintptr_t*)((uintptr_t)&pmm_available_start - 0xC0000000);
    *avail_start = (uintptr_t)&__kernel_phys_end;

    multiboot_info_t* mb_info_ptr_phys = *((multiboot_info_t**)((uintptr_t)&mb_info - 0xC0000000));
    multiboot_info_t* mb_info_phys = (multiboot_info_t*)((uintptr_t)mb_info_ptr_phys - 0xC0000000);
    
    /* Multiboot info structure */
    uintptr_t mb_info_end = (uintptr_t)mb_info_phys + sizeof(multiboot_info_t);
    if (mb_info_end > *avail_start) *avail_start = mb_info_end;

    /* command line */
    if ((mb_info_phys->flags & MULTIBOOT_INFO_CMDLINE) && mb_info_phys->cmdline) {
        const char* cmdline = (const char*)mb_info_phys->cmdline;
        uintptr_t cmdline_end = mb_info_phys->cmdline + strlen_lh(cmdline) + 1;
        if (cmdline_end > *avail_start) *avail_start = cmdline_end;
    }

    /* boot modules */
    if ((mb_info_phys->flags & MULTIBOOT_INFO_MODS) && mb_info_phys->mods_addr && mb_info_phys->mods_count) {
        uintptr_t mods_end = mb_info_phys->mods_addr + mb_info_phys->mods_count * sizeof(multiboot_module_t);
        if (mods_end > *avail_start) *avail_start = mods_end;

        const multiboot_module_t* module = (const multiboot_module_t*)mb_info_phys->mods_addr;
        for (size_t i = 0; i < mb_info_phys->mods_count; i++, module++) {
            if (module->mod_end > *avail_start) *avail_start = module->mod_end;
            if (module->cmdline) {
                const char* cmdline = (const char*)module->cmdline;
                uintptr_t cmdline_end = module->cmdline + strlen_lh(cmdline) + 1;
                if (cmdline_end > *avail_start) *avail_start = cmdline_end;
            }
        }
    }

    /* memory map */
    if ((mb_info_phys->flags & MULTIBOOT_INFO_MEM_MAP) && mb_info_phys->mmap_addr && mb_info_phys->mmap_length) {
        uintptr_t end = mb_info_phys->mmap_addr + mb_info_phys->mmap_length;
        if (end > *avail_start) *avail_start = end;
    }

    /* drive info */
    if ((mb_info_phys->flags & MULTIBOOT_INFO_DRIVE_INFO) && mb_info_phys->drives_addr && mb_info_phys->drives_length) {
        uintptr_t end = mb_info_phys->drives_addr + mb_info_phys->drives_length;
        if (end > *avail_start) *avail_start = end;
    }

    *avail_start += 0xC0000000;
}

void pmm_target_init(void) {
    pmm_machine_init();

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
