#include <i386-pc-multiboot/multiboot.h>
#include <kernel/log.h>

/* Multiboot header */
#define MULTIBOOT_FLAGS                             (MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO)
__attribute__((section(".multiboot")))
struct multiboot_header multiboot_header = {
    .magic = MULTIBOOT_HEADER_MAGIC,
    .flags = MULTIBOOT_FLAGS,
    .checksum = -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_FLAGS)
};

void mb_print_mmap(void) {
    multiboot_memory_map_t* mmap = (multiboot_memory_map_t*)mb_info->mmap_addr;
    for (int i = 0; mmap; mmap = mb_traverse_mmap(mmap), i++) {
        LOG_INFO("mmap entry %d: addr 0x%08x%08x size 0x%08x%08x type %u", i, mmap->addr_h, mmap->addr_l, mmap->len_h, mmap->len_l, mmap->type);
    }
}

size_t mb_get_contiguous_mem_size(void) {
    multiboot_memory_map_t* mmap = (multiboot_memory_map_t*)mb_info->mmap_addr;
    size_t start_addr = 0;
    while (mmap) {
        if (mmap->addr_l)
        mmap = mb_traverse_mmap(mmap);
    }
    return start_addr;
}
