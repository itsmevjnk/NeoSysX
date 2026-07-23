#include <cpu/halt.h>
#include <kernel/log.h>

__attribute__((weak)) void cpu_reset(void) {
    LOG_WARN("target BSP does not implement cpu_reset(); using cpu_halt() instead");
    cpu_halt();
}
