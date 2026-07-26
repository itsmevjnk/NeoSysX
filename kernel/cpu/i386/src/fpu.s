.section .bss
.global fpu_mode
fpu_mode:
    .skip 4

.section .text    
.align 16
.global fpu_init
.type idt_fpu_probe, @function
fpu_init:
    pushl %ebp
    movl %esp, %ebp

    # clear EM and ET to force FPU access
    movl %cr0, %eax
    andl $(~((1 << 2) | (1 << 4))), %eax
    movl %eax, %cr0

    # initialise fpu_mode to 0
    xor %eax, %eax
    movl %eax, fpu_mode

    movw $0x55AA, %ax
    fninit
    fnstsw %ax
    cmpw $0, %ax
    jne .fin

    # FPU exists
    inc %eax # EAX = 1 (was 0 previously if we could get to this point)
    movl fpu_mode, %eax

    # check if CPUID is available; if not then FXSAVE is definitely not available
    pushfl
    pushfl
    xorl $(1 << 21), (%esp) # invert ID bit
    popfl
    pushfl
    popl %eax
    xor %eax, (%esp) # EAX contains bits that changed
    popfl
    testl $(1 << 21), %eax
    jnz .fin # ID bit can be changed, so CPUID is not supported

    # check if FXSAVE is supported
    pushfl
    xor %eax, %eax
    inc %eax
    cpuid
    test $(1 << 24), %edx
    popfl
    jnz .fin # FXSAVE is not supported

    # FXSAVE is supported - enable it
    movl %cr4, %eax
    orl $(1 << 9), %eax # set OSFXSR to save XMM0..7
    movl %eax, %cr4

    xor %eax, %eax
    inc %eax
    inc %eax # EAX = 2
    movl %eax, fpu_mode

.fin:
    popl %ebp
    ret