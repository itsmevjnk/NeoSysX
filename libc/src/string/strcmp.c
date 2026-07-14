#include <string.h>

__attribute__((weak))
int strcmp(const char* lhs, const char* rhs) {
    // NOTE: we can instead call strncmp(lhs, rhs, SIZE_MAX)
    int ret = 0;
    for (size_t i = 0; (lhs[i] || rhs[i]) && !ret; i++) {
        ret = lhs[i] - rhs[i];
    }
    return ret;
}