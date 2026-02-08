.section .text
.global _start
_start:
    la sp, stack_top      # Set up the stack pointer
    call kmain            # Jump to our C code
loop:
    wfi                   # Wait for Interrupt (saves CPU)
    j loop                # Infinite loop if C returns

.section .bss
.align 16
stack_low:
    .skip 4096            # 4KB of stack space
stack_top:
