#ifndef __CPU_I386_FPU_H
#define __CPU_I386_FPU_H

#include <stddef.h>
#include <stdint.h>

extern uint32_t fpu_mode; // 0 = no FPU, 1 = FPU detected, only FSAVE supported, 2 = FPU detected, FXSAVE supported

void fpu_init(void); // probe for FPU/SSE and initialise

#endif
