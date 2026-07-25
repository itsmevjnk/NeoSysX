#include <algo/idalloc.h>
#include <stdlib.h>
#include <string.h>

#ifndef IDALLOC_EXPAND_INCREMENT
#include <mm/pmm.h>
#define IDALLOC_EXPAND_INCREMENT pmm_get_frame_size()
#endif

bool idalloc_init(idalloc_t* base) {
    if (base->is_static) return true; // nothing to initialise here
    
    size_t remainder = base->size % sizeof(size_t);
    if (base->size % sizeof(size_t)) base->size += sizeof(size_t) - remainder;
    size_t increment = IDALLOC_EXPAND_INCREMENT;
    remainder = base->size % increment;
    if (remainder) base->size += increment - remainder;
    
    base->next_to_allocate = 0;
    base->bitmap = (size_t*)calloc(base->size, 1);
    if (!base->bitmap) return false;
    return true;
}

#define SIZE_BITS (sizeof(size_t) * 8)

size_t idalloc_alloc(idalloc_t* base) {
    size_t id = base->next_to_allocate;
    while (true) {
        size_t idx = id / SIZE_BITS, bit = id % SIZE_BITS;
        if (idx * sizeof(size_t) >= base->size) break;
        if (!(base->bitmap[idx] & (1 << bit))) {
            base->bitmap[idx] |= (1 << bit);
            base->next_to_allocate = id + 1;
            return id;
        }
        id++;
    }
    
    /* allocate more memory for bitmap if possible */
    if (!idalloc_expand(base, id)) return SIZE_MAX;
    base->next_to_allocate = id; // bootstrap ourselves
    return idalloc_alloc(base); // try again
}

void idalloc_free(idalloc_t* base, size_t id) {
    size_t idx = id / SIZE_BITS, bit = id % SIZE_BITS;
    if (idx * sizeof(size_t) >= base->size) return;
    base->bitmap[idx] &= ~(1 << bit);
}

bool idalloc_expand(idalloc_t* base, size_t max_id) {
    if (base->is_static) return false; // cannot expand

    size_t new_size = ((max_id + 1 + sizeof(size_t) - 1) / sizeof(size_t)) * sizeof(size_t);
    if (base->size > new_size) return true; // big enough

    // align new_size
    size_t increment = IDALLOC_EXPAND_INCREMENT;
    size_t remainder = new_size % increment;
    if (remainder) new_size += increment - remainder;

    uint8_t* new_bitmap = (uint8_t*)realloc((void*)base->bitmap, new_size);
    if (!new_bitmap) return false;
    memset(&new_bitmap[base->size], 0, new_size - base->size);
    base->bitmap = (size_t*)new_bitmap;
    base->size = new_size;
    return true;
}

void idalloc_trim(idalloc_t* base) {
    int i = base->size / sizeof(size_t) - 1;
    for (; i >= 0 && !base->bitmap[i]; i--); // scan backward until we get to non-zero bitmap entry
    i++;

    size_t new_size = i * sizeof(size_t);
    size_t increment = IDALLOC_EXPAND_INCREMENT;
    if (new_size < increment) new_size = increment;    
    else {
        size_t remainder = new_size % increment;
        if (remainder) new_size += increment - remainder;
    }

    size_t* new_bitmap = (size_t*)realloc((void*)base->bitmap, new_size);
    if (!new_bitmap) return; // cannot trim
    base->bitmap = new_bitmap;
    base->size = new_size;
}

void idalloc_deinit(idalloc_t* base) {
    if (base->is_static) return; // cannot deallocate statically allocated heap
    free((void*)base->bitmap);
    base->size = 0; base->bitmap = NULL;
}