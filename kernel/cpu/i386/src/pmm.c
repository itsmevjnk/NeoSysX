#include <cpu/i386/pmm.h>
#include <cpu/i386/vmm.h>

uintptr_t pmm_available_start = 0;
uintptr_t pmm_get_available_start(void) {
    return pmm_available_start;
}

#ifndef PMM_LEVELS
#define PMM_LEVELS                          4 // number of buddy allocator levels
#endif

#define SIZE_BITS (sizeof(size_t) * 8)

// project bitmap size for reservation during _start_lh
__attribute__((section(".text.lh")))
size_t pmm_get_bitmap_size_lh(void) {
    uintptr_t start_addr = *((uint32_t*)((uintptr_t)&pmm_available_start - 0xC0000000));
    uintptr_t bitmap_addr = start_addr;
    if (bitmap_addr & 0xFFF) bitmap_addr = (bitmap_addr & ~0xFFF) + 0x1000; // align start address

    size_t mem_size = pmm_get_size_lh();
    uintptr_t num_frames = mem_size >> 12; if (mem_size & 0xFFF) num_frames++;
    for (int level = 0, bits = num_frames; level < PMM_LEVELS && bits; level++, bits = (bits + 1) >> 1) {
        size_t level_size = (bits + SIZE_BITS - 1) / SIZE_BITS;
        bitmap_addr += level_size * sizeof(size_t);
    }

    if (bitmap_addr & 0xFFF) bitmap_addr = (bitmap_addr & ~0xFFF) + 0x1000; // align end address
    return bitmap_addr - start_addr;
}

void pmm_cpu_init(void) {
    vmm_reserve_initial_pt();
}
