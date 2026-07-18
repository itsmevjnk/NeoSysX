#include <mm/pmm.h>
#include <kernel/log.h>
#include <string.h>

#ifndef PMM_LEVELS
#define PMM_LEVELS                          4 // number of buddy allocator levels
#endif
static size_t* pmm_bitmaps[PMM_LEVELS] = {NULL};
static size_t pmm_bitmap_sizes[PMM_LEVELS] = {0}; // NOTE: in size_t units, not bytes

#ifndef PMM_FRAME_SIZE
#define PMM_FRAME_SIZE                      4096 // common among x86 (and maybe ARM?)
#endif

#define SIZE_BITS                           (8 * sizeof(size_t))

static uint64_t pmm_max_size = UINT64_MAX;

/* expected from linker script */
extern uintptr_t __kernel_start;
extern uintptr_t __kernel_end;

void pmm_init(void) {
    pmm_max_size = pmm_get_size();
    size_t num_frames = (pmm_max_size + PMM_FRAME_SIZE - 1) / PMM_FRAME_SIZE;
    LOG_INFO("building bitmaps for %lu phys frames", num_frames);

    uintptr_t start_addr = pmm_get_available_start();
    if (start_addr % PMM_FRAME_SIZE) start_addr += PMM_FRAME_SIZE - start_addr % PMM_FRAME_SIZE; // align start address
    LOG_DEBUG("bitmap start address: 0x%lx", start_addr);

    uintptr_t bitmap_addr = start_addr;
    for (int level = 0, bits = num_frames; level < PMM_LEVELS && bits; level++, bits = (bits + 1) >> 1) {
        size_t level_size = (bits + SIZE_BITS - 1) / SIZE_BITS;
        pmm_bitmaps[level] = (size_t*)bitmap_addr;
        pmm_bitmap_sizes[level] = level_size;
        bitmap_addr += level_size * sizeof(size_t);
        // LOG_DEBUG("level %d has %d frames, level size = %lu", level, bits, level_size);
    }
    LOG_DEBUG("bitmap end address: 0x%lx (%lu bytes)", bitmap_addr, bitmap_addr - start_addr);
    memset((void*)start_addr, 0, bitmap_addr - start_addr);

    uint64_t max_addr = (uint64_t)pmm_bitmap_sizes[0] * SIZE_BITS * PMM_FRAME_SIZE;
    LOG_DEBUG("theoretical max addr: 0x%llx, mem size: 0x%llx", max_addr, pmm_max_size);
    pmm_reserve(pmm_max_size, max_addr - pmm_max_size); // reserve unusable space beyond
    pmm_reserve(start_addr, bitmap_addr - start_addr); // also reserve bitmap space
    pmm_reserve((uintptr_t)&__kernel_start, (uintptr_t)&__kernel_end - (uintptr_t)&__kernel_start); // reserve kernel space
    pmm_target_init();
}

size_t pmm_get_frame_size(void) {
    return PMM_FRAME_SIZE;
}

void pmm_reserve(uint64_t addr, uint64_t len) {
    if (addr > pmm_max_size) return;
    else if (addr + len > pmm_max_size) len = pmm_max_size - addr;

    for (size_t level = 0, block_len = PMM_FRAME_SIZE; level < PMM_LEVELS && pmm_bitmaps[level]; level++, block_len <<= 1) {
        size_t remainder = addr % block_len;
        addr -= remainder;
        len += remainder;

        uint64_t frame_idx = addr / block_len;
        uint64_t n_frames = (len + block_len - 1) / block_len;
        size_t first_bit_start = 0, first_bit_count = 0, last_bit_count = 0;
        size_t fill_idx = frame_idx / SIZE_BITS, fill_cnt = 0;
        if (frame_idx % SIZE_BITS) {
            fill_idx++;
            first_bit_start = frame_idx % SIZE_BITS;
            first_bit_count = SIZE_BITS - first_bit_start;
            if (first_bit_count > n_frames) first_bit_count = n_frames;
            n_frames -= first_bit_count;
        }
        fill_cnt = n_frames / SIZE_BITS;
        last_bit_count = n_frames - fill_cnt * SIZE_BITS;
        
        if (first_bit_count) {
            size_t mask = ((1 << (first_bit_count + 1)) - 1) << first_bit_start;
            pmm_bitmaps[level][frame_idx / SIZE_BITS] |= mask;
        }
        if (fill_cnt) memset(&pmm_bitmaps[level][fill_idx], 0xFF, fill_cnt * sizeof(size_t));
        if (last_bit_count) {
            size_t mask = (1 << (last_bit_count + 1)) - 1;
            pmm_bitmaps[level][fill_idx + fill_cnt] |= mask;
        }
    }
}

uint64_t pmm_alloc(uint64_t len) {
    /* find smallest block that corresponds to len */
    int level = 0;
    size_t block_len = PMM_FRAME_SIZE;
    for (; block_len < len && level < PMM_LEVELS - 1 && pmm_bitmaps[level]; level++, block_len <<= 1);

    /* search for contiguous block */
    for (; level >= 0; level--, block_len >>= 1) { // step one level down if we cannot find a block
        size_t req_bits = (len + block_len - 1) / block_len; // number of consecutive bits that must be clear
        size_t total_bits = pmm_bitmap_sizes[level] * sizeof(size_t) * 8;
        size_t bit = 0, word_idx = 0, bit_idx = 0;
        size_t clear_bits = 0;
        for (; bit < total_bits; bit++, bit_idx++) {
            if (bit_idx == 8 * sizeof(size_t)) {
                bit_idx = 0;
                word_idx++;
            }
            if (!bit_idx) { // skip words without any clear bits
                while (!(~pmm_bitmaps[level][word_idx])) {
                    word_idx++;
                    bit += 8 * sizeof(size_t);
                    if (bit >= total_bits) break;
                }
                if (bit >= total_bits) break;
            }
            if (!(pmm_bitmaps[level][word_idx] & (1 << bit_idx))) {
                clear_bits++;
                if (clear_bits == req_bits) { // bingo!
                    size_t start_bit = bit - clear_bits + 1;
                    uint64_t addr = start_bit * block_len;
                    pmm_reserve(addr, len);
                    return addr;
                }
            } else clear_bits = 0;
        }
    }

    return UINTPTR_MAX;
}

void pmm_free(uint64_t addr, uint64_t len) {
    if (addr > pmm_max_size) return;
    else if (addr + len > pmm_max_size) len = pmm_max_size - addr;

    // TODO: reimplement this to be more efficient

    size_t remainder = addr % PMM_FRAME_SIZE;
    addr -= remainder;
    len += remainder;

    size_t frame_idx = addr / PMM_FRAME_SIZE;
    size_t n_frames = (len + PMM_FRAME_SIZE - 1) / PMM_FRAME_SIZE;
    for (size_t frame = 0; frame < n_frames; frame++, frame_idx++) {
        size_t word_idx = frame_idx / (8 * sizeof(size_t));
        size_t bit_idx = frame_idx % (8 * sizeof(size_t));
        pmm_bitmaps[0][word_idx] &= ~(1 << bit_idx);

        /* propagate bit change to larger levels */
        for (size_t level = 1, bit = frame_idx >> 1; level < PMM_LEVELS && pmm_bitmaps[level]; level++, bit >>= 1) {
            /* compute index for smaller level */
            word_idx = (bit << 1) / (8 * sizeof(size_t));
            bit_idx = (bit << 1) % (8 * sizeof(size_t)); // bit_idx and bit_idx + 1 must be clear
            if (pmm_bitmaps[level - 1][word_idx] & (0b11 << bit_idx)) break; // cannot propagate further

            /* compute index for this level */
            word_idx = bit / (8 * sizeof(size_t));
            bit_idx = bit % (8 * sizeof(size_t));
            pmm_bitmaps[level][word_idx] &= ~(1 << bit_idx);
        }
    }
}
