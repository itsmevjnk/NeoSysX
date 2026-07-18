#include <kernel/main.h>
#include <machine/i386-pc/init.h>
#include <i386-pc-multiboot/multiboot.h>

void kinit_target_pre(void) {
    kinit_machine_pre();
}

void kinit_target_post(void) {
    kinit_machine_post();
    mb_print_mmap();
}