#include <cpu/i386/init.h>

#include <cpu/i386/gdt.h>
#include <cpu/i386/idt.h>

#include <cpu/intr.h>

void kinit_cpu_pre(void) {
    gdt_init();
    idt_init();

    intr_enable(); // TODO: remap PIC (otherwise we'll be hit with a bogus #DF right after this)
}

void kinit_cpu_post(void) {

}
