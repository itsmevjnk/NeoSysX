#include <main.h>
#include <machine/i386-pc/init.h>

void kinit_target_pre(void) {
    kinit_machine_pre();
}

void kinit_target_post(void) {
    kinit_machine_post();
}