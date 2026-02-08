.section .text
.global _start
_start:
    la sp, stack_top      # Set up the stack pointer
    la t0, trap_entry     # Set up the trap handler
    csrw mtvec, t0        # Set the trap vector to our trap handler
    call kmain            # Jump to our C code
loop:
    wfi                   # Wait for Interrupt (saves CPU)
    j loop                # Infinite loop if C returns
.align 4
trap_entry:
    j handle_trap
.section .bss
.align 16
stack_low:
    .skip 4096            # 4KB of stack space
stack_top:
