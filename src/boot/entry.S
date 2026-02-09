.section .text
.global _start
_start:
    la sp, stack_top      # Set up the stack pointer
    la t0, trap_entry     # Set up the trap handler
    csrw mtvec, t0        # Set the trap vector to our trap handler
    li t0, 0x00006000     # Load the mask for FS bits. enable float
    csrs mstatus, t0      # Set the FS bits to 11 (Dirty/Initial). enable float
    call kmain            # Jump to our C code
loop:
    wfi                   # Wait for Interrupt (saves CPU)
    j loop                # Infinite loop if C returns
.align 4
trap_entry:
    csrr a0, mcause    # Argument 1: mcause
    csrr a1, mepc      # Argument 2: mepc
    j handle_trap
.section .bss
.align 16
stack_low:
    .skip 4096            # 4KB of stack space
stack_top:
