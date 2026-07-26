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
.extern fpu_mode
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

    # push FPU state
    subl $(464 + 16), %esp # reserve space for fsave_buf
    movl %esp, %edi # EDI = fsave_buf pointer
    movl fpu_mode, %eax
    cmpl $2, %eax
    je .fxsave
    cmpl $1, %eax
    je .fsave
    # unsupported
    pushl $0 # fsave_ptr = 0
    jmp .fpu_save_done

.fxsave: # use FXSAVE
    test $0b1111, %edi # check if the start of fsave_buf is aligned already
    jz .fxsave_aligned
    # make EDI 16-byte aligned
    andl $(~0b1111), %edi
    addl $0x10, %edi
.fxsave_aligned:
    fxsave (%edi)
    jmp .save_fsave_buf_addr

.fsave: # use FSAVE
    fsave (%edi)

.save_fsave_buf_addr:
    pushl %edi # save address
.fpu_save_done:

    pushl %esp
    call idt_handler_stub # pass to C code
    add $4, %esp

.global idt_restore_ctx
.type idt_restore_ctx, @function
idt_restore_ctx:
    # pop FPU state
    popl %edi # EDI = fsave_buf pointer containing FPU state pushed previously
    movl fpu_mode, %eax
    cmpl $2, %eax
    je .fxrstor
    cmpl $1, %eax
    je .frstor
    # unsupported
    pushl $0 # fsave_ptr = 0
    jmp .fpu_restore_done
.fxrstor:
    fxrstor (%edi)
    jmp .fpu_restore_done
.frstor:
    frstor (%edi)

.fpu_restore_done:
    addl $(464 + 16), %esp

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