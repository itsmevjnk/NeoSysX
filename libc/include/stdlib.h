#ifndef __LIBC_STDLIB_H
#define __LIBC_STDLIB_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* from liballoc */
void* malloc(size_t size);
void* realloc(void* ptr, size_t size);
void* calloc(size_t num, size_t size);
void free(void* ptr);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __LIBC_STDLIB_H */