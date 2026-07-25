#ifndef __ALGO_IDALLOC_H
#define __ALGO_IDALLOC_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    size_t size; // bitmap size in bytes
    size_t* bitmap;
    size_t next_to_allocate; // next ID to allocate
    bool is_static; // set if bitmap is statically pre-allocated and can thus not be deallocated
} idalloc_t;

bool idalloc_init(idalloc_t* base); // initialise bitmap based on the initial bitmap size set in size
size_t idalloc_alloc(idalloc_t* base); // allocate an ID
void idalloc_free(idalloc_t* base, size_t id); // release an ID
bool idalloc_expand(idalloc_t* base, size_t max_id); // expand the bitmap to store at least up to the specified max ID
void idalloc_trim(idalloc_t* base); // trim the bitmap to the minimum possible size
void idalloc_deinit(idalloc_t* base); // deallocate node bitmap

#endif