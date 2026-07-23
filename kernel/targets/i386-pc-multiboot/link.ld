ENTRY(_start)

SECTIONS {
    ENTRY(_start_lh)

    . = 0x100000;

    /* LOWER HALF */
    __kernel_lh_phys_start = .;
    __kernel_lh_start = . + 0xC0000000;
    .text.lh BLOCK(4K) : ALIGN(4K) {
        . = ALIGN(8);
        __multiboot_start = .;
        KEEP(*(.multiboot))
        __multiboot_end = .;

        __text_lh_start = .;
        *(.text.lh)
        __text_lh_end = .;
    }
    __kernel_lh_phys_end = .;
    __kernel_lh_end = . + 0xC0000000;

    . += 0xC0000000;

    /* HIGHER HALF */
    __kernel_start = .;
    __kernel_phys_start = . - 0xC0000000;

    .text ALIGN(4K) : AT(ADDR(.text) - 0xC0000000) {
        __text_start = .;
        __text_phys_start = . - 0xC0000000;
        *(.text)
        __text_end = .;
        __text_phys_end = . - 0xC0000000;
    }

    .rodata ALIGN(4K) : AT(ADDR(.rodata) - 0xC0000000) {
        __rodata_start = .;
        __rodata_phys_start = . - 0xC0000000;
        *(.rodata)
        __rodata_end = .;
        __rodata_phys_end = . - 0xC0000000;
    }

    .data ALIGN(4K) : AT(ADDR(.data) - 0xC0000000) {
        __data_start = .;
        __data_phys_start = . - 0xC0000000;
        *(.data)
        __data_end = .;
        __data_phys_end = . - 0xC0000000;
    }

    .bss ALIGN(4K) : AT(ADDR(.bss) - 0xC0000000) {
        __bss_start = .;
        __bss_phys_start = . - 0xC0000000;
        *(COMMON)
        *(.rodata)
        __bss_end = .;
        __bss_phys_end = . - 0xC0000000;
    }

    __kernel_end = .;
    __kernel_phys_end = . - 0xC0000000;
}