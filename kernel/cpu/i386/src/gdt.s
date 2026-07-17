.section .text
.global gdt_init
.extern gdt_desc # implemented in gdt.c
.type gdt_init, @function
gdt_init:
    # function prologue
    pushl %ebp
    movl %esp, %ebp
    
    lgdt gdt_desc

    # reload CS
    jmpl $0x08, $.reload_cs
.reload_cs:
    # reload DS
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss

    # function epilogue
    popl %ebp
    ret