#include <cpu/halt.h>

void cpu_halt(void) {
    __asm__ volatile("hlt");
}
