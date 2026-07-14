.section .bss
.align 16
stack_bottom:
.skip 16384
stack_top:

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
