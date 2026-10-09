#ifndef __ARCH_HALT_H
#define __ARCH_HALT_H

static inline void cpu_halt(void) {
    __asm__ volatile("hlt");
}

#endif