.section .bss
.align 16
stack_bottom:
.skip 16384
stack_top:

.section .text
.global _start
.extern _init
.extern kmain
.extern mb_info
.type _start, @function
_start:
    movl $stack_top, %esp

    cmp $0x2BADB002, %eax
    jne .init
    movl %ebx, mb_info
.init:

    call _init
    call kmain

    # UNREACHABLE
    cli
1:  hlt
    jmp 1b
.size _start, . - _start
