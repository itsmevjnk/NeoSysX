#ifndef __ARCH_INTR_H
#define __ARCH_INTR_H

#include <stdbool.h>
#include <stdint.h>

static inline void intr_enable(void) {
    asm volatile("sti" ::: "memory");
}

static inline void intr_disable(void) {
    asm volatile("cli" ::: "memory");
}

static inline bool intr_enabled(void) {
    uintptr_t flags; // EFLAGS/RFLAGS
    asm volatile("pushf; popl %0" : "=r"(flags) :: "memory");
    return flags & (1 << 9);
}

#endif
