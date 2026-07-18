#include <i386-pc-multiboot/multiboot_spec.h>

struct multiboot_info* mb_info = NULL;

struct multiboot_mmap_entry* mb_traverse_mmap(struct multiboot_mmap_entry* prev) {
    if (!mb_info) return NULL; // Multiboot info is not ready yet
    if (!prev) return (struct multiboot_mmap_entry*)mb_info->mmap_addr;

    prev = (struct multiboot_mmap_entry*)((uintptr_t)prev + prev->size + sizeof(prev->size));
    if ((uintptr_t)prev < mb_info->mmap_addr || (uintptr_t)prev >= mb_info->mmap_addr + mb_info->mmap_length) return NULL;
    return prev;
}