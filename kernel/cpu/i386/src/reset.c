#include <cpu/halt.h>
#include <cpu/i386/idt.h>

__attribute__((noreturn)) void cpu_reset(void) {
    /* reset by IDT trashing */
    static const idt_descriptor_t dummy_desc = {0};
    __asm__ volatile("cli"); // probably not needed
    __asm__ volatile("lidt %0" : : "m"(dummy_desc));
    __asm__ volatile("int $0x80"); // or any interrupt here, doesn't matter

    while (1) {
        __asm__ volatile("hlt");
    }
}