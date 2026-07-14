#include <string.h>

__attribute__((weak))
int memcmp(const void* lhs, const void* rhs, size_t count) {
    const char* lhs_char = (const char*) lhs;
    const char* rhs_char = (const char*) rhs;

    int ret = 0;
    for (size_t i = 0; i < count && !ret; i++) {
        ret = lhs_char[i] - rhs_char[i];
    }
    return ret;
}