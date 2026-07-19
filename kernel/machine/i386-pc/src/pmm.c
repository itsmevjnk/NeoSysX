#include <machine/i386-pc/pmm.h>
#include <cpu/i386/pmm.h>
#include <mm/pmm.h>

void pmm_machine_init(void) {
    pmm_cpu_init();

    /* reserve BIOS-related memory regions in the first 1MB */
    pmm_reserve(0x00000, 0x004FF - 0x00000 + 1); // real mode IVT and BDA
    pmm_reserve(0x80000, 0x9FFFF - 0x80000 + 1); // EBDA
    pmm_reserve(0xA0000, 0xBFFFF - 0xA0000 + 1); // video memory
    pmm_reserve(0xC0000, 0xC7FFF - 0xC0000 + 1); // video BIOS
    pmm_reserve(0xC8000, 0xEFFFF - 0xC8000 + 1); // BIOS expansions
    pmm_reserve(0xF0000, 0xFFFFF - 0xF0000 + 1); // BIOS
}