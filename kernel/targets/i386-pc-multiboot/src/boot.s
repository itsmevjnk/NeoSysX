.section .bss
.global stack_top
.align 16
stack_bottom:
.skip 16384
stack_top:

.section .text.lh
.global kinit_target_lh
.type kinit_target_lh, @function
.extern mb_info
kinit_target_lh:
    pushl %ebp
    movl %esp, %ebp
    cmpl $0x2BADB002, %eax
    jne .done
    addl $0xC0000000, %ebx
    movl %ebx, (mb_info - 0xC0000000)
.done:
    popl %ebp
    ret

.section .text
.global _start
.extern _init
.extern kmain
.type _start, @function
_start:
    movl $stack_top, %esp

    call _init
    call kmain

    # UNREACHABLE
    cli
1:  hlt
    jmp 1b
.size _start, . - _start
