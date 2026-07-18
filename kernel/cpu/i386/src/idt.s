.section .text

.macro idt_handler_err intnum
    .align 16
    .global idt_handler_\intnum
    .type idt_handler_\intnum, @function
    idt_handler_\intnum:
        pushl $\intnum
        jmp idt_handler_asmstub
.endm

.macro idt_handler_noerr intnum
    .align 16
    .global idt_handler_\intnum
    .type idt_handler_\intnum, @function
    idt_handler_\intnum:
        pushl $0 # dummy error code
        pushl $\intnum
        jmp idt_handler_asmstub
.endm

# exception handlers
idt_handler_noerr 0
idt_handler_noerr 1
idt_handler_noerr 2
idt_handler_noerr 3
idt_handler_noerr 4
idt_handler_noerr 5
idt_handler_noerr 6
idt_handler_noerr 7
idt_handler_err 8
idt_handler_noerr 9
idt_handler_err 10
idt_handler_err 11
idt_handler_err 12
idt_handler_err 13
idt_handler_err 14
idt_handler_noerr 15 # reserved
idt_handler_noerr 16
idt_handler_err 17
idt_handler_noerr 18
idt_handler_noerr 19
idt_handler_noerr 20
idt_handler_err 21
# reserved
idt_handler_noerr 22
idt_handler_noerr 23
idt_handler_noerr 24
idt_handler_noerr 25
idt_handler_noerr 26
idt_handler_noerr 27
idt_handler_noerr 28
idt_handler_noerr 29
idt_handler_noerr 30
idt_handler_noerr 31

# other interrupt handlers
.altmacro
.set num, 32
.rept 224
    idt_handler_noerr %num
    .set num, num + 1
.endr

# IDT handler stub
.align 16
.extern idt_handler_stub
.type idt_handler_asmstub, @function
idt_handler_asmstub:
    pushal # push EAX..EDI
    
    # push DS..GS
    xor %eax, %eax
    mov %ds, %ax
    pushl %eax
    mov %es, %ax
    pushl %eax
    mov %fs, %ax
    pushl %eax
    mov %gs, %ax
    pushl %eax

    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs

    pushl %esp
    call idt_handler_stub # pass to C code
    add $4, %esp

    popl %eax
    mov %ax, %gs
    popl %eax
    mov %ax, %fs
    popl %eax
    mov %ax, %es
    popl %eax
    mov %ax, %ds

    popal
    add $8, %esp # skip vector and errcode
    iret
    