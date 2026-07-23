.section .text.lh

# this function sets up paging in lower half, before handing control to _start in higher half.
.global _start_lh
.extern _start
.extern stack_top
.extern kinit_target_lh
.extern vmm_lh_init
.extern vmm_kernel_config
.type _start_lh, @function
_start_lh:
    movl $(stack_top - 0xC0000000), %esp # set stack to the space that we manage
    pushal # preserve all registers as we're going to tamper with them

    # call target initialisation in lower half
    call kinit_target_lh

    # populate page directory and allocate page tables
    call vmm_lh_init

    # enable paging
    movl $(vmm_kernel_config - 0xC0000000), %eax
    movl %eax, %cr3
    movl %cr4, %eax
    orl $(1 << 4), %eax # enable PSE bit for 4MB pages
    movl %eax, %cr4
    movl %cr0, %eax
    orl $((1 << 31) | (1 << 0)), %eax # enable PG bit for paging, and ensure PE is already on
    movl %eax, %cr0
    
    popal
    jmp _start # long jmp to higher half
