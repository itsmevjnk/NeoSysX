#ifndef __CPU_HALT_H
#define __CPU_HALT_H

#include <stddef.h>
#include <stdint.h>

void cpu_halt(void); // halt until an interrupt is received (HLT in x86, WFI/WFE in ARM)
__attribute__((noreturn)) void cpu_reset(void); // reset the CPU using the most basic type of hardware method available (i.e. no ACPI)

#endif /* __CPU_HALT_H */