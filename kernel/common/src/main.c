#include <main.h>

/* placeholder */
__attribute__((weak)) void kinit_target_pre(void) {

}

__attribute__((weak)) void kinit_target_post(void) {

}

void kmain(void) {
    kinit_target_pre();

    // TODO: initialise kernel
    
    kinit_target_post();

    while (1);
}