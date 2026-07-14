#include <string.h>
#include <stdint.h>

__attribute__((weak))
void* memmove(void* dest, const void* src, size_t count) {
    char* dest_char = (char*) dest;
    const char* src_char = (char*) src;
    
    if ((uintptr_t) dest > (uintptr_t) src) {
        if (!count) return dest; // do nothing
        for (size_t i = count - 1; /* termination condition implemented below*/; i++) {
            dest_char[i] = src_char[i];
            if (!i) break;
        }
    } else {
        for (size_t i = 0; i < count; i++) {
            dest_char[i] = src_char[i];
        }    
    }
    return dest;
}