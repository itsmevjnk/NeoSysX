#ifndef __CPU_INTR_H
#define __CPU_INTR_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/* interrupt functionalities to be implemented by the CPU BSP */
void intr_enable(void);
void intr_disable(void);
bool intr_enabled(void);

/* macros for implementing interrupt disable guard */
#define INTR_ENSURE_DISABLED_BEGIN \
{ \
    bool __is_intr_enabled = intr_enabled(); \
    if (__is_intr_enabled) intr_disable(); \

#define INTR_ENSURE_DISABLED_END \
    if (__is_intr_enabled) intr_enable(); \
}

#endif