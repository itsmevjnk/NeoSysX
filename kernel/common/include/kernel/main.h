#ifndef __KERNEL_MAIN_H
#define __KERNEL_MAIN_H

#include <stddef.h>
#include <stdint.h>

// target-specific kernel init functions
void kinit_target_pre(void);
void kinit_target_post(void);

void kmain(void);

#endif /* __KERNEL_MAIN_H */