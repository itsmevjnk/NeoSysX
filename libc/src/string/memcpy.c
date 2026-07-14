#include <string.h>

__attribute__((weak))
void* memcpy(void* dest, const void* src, size_t count) {
    char* dest_char = (char*) dest;
    const char* src_char = (char*) src;
    for (size_t i = 0; i < count; i++) {
        dest_char[i] = src_char[i];
    }    
    return dest;
}