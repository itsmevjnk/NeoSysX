#ifndef __CPU_INTR_H
#define __CPU_INTR_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/* interrupt functionalities to be implemented by the CPU BSP */
void intr_enable(void);
void intr_disable(void);
bool intr_enabled(void);

#endif