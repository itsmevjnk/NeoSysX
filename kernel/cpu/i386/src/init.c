#include <cpu/i386/init.h>

#include <cpu/i386/gdt.h>

void kinit_cpu_pre(void) {
    gdt_init();
}

void kinit_cpu_post(void) {

}
