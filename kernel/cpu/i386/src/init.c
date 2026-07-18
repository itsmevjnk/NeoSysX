#include <cpu/i386/init.h>

#include <cpu/i386/gdt.h>
#include <cpu/i386/idt.h>

void kinit_cpu_pre(void) {
    gdt_init();
    idt_init();
}

void kinit_cpu_post(void) {

}
