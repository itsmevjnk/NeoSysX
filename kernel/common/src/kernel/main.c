#include <kernel/main.h>
#include <kernel/log.h>

#include <mm/pmm.h>

/* placeholder */
__attribute__((weak)) void kinit_target_pre(void) {

}

__attribute__((weak)) void kinit_target_post(void) {

}

void kmain(void) {
    LOG_DEBUG("entering kinit_target_pre");
    kinit_target_pre();

    // TODO: initialise kernel
    pmm_init();
    
    LOG_DEBUG("entering kinit_target_post");
    kinit_target_post();

    LOG_INFO("kernel init complete");
    while (1);
}