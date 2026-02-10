.section .text.boot
.global _start
_start:
    # 1. Set up the stack pointer using the symbol from our linker script
    la sp, stack_top      

    # 2. Set up the trap handler (pointing to the one in traps.S)
    la t0, trap_entry     
    csrw mtvec, t0        

    # 3. Enable FPU (Floating Point)
    li t0, 0x00006000     
    csrs mstatus, t0      

    # 4. Jump to C
    call kmain            

loop:
    wfi                   
    j loop