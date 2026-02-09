#include <stddef.h>
#include <stdint.h>
#include <drivers/uart.h>
#include <syscon/syscon.h>

void kpanic(const char *reason){
    kputs("\n!!! PANIC !!!\n");
	kputs(reason);
    kputs("\n!!! PANIC !!!\n");
	poweroff();
}

void kpanic_force() {
    kprint("\n!!! FORCE PANIC !!!\n");

    // 'ebreak' is the standard RISC-V way to trigger a debug trap.
    __asm__ volatile("ebreak");

    kprint("Force panic falled. Power off. \n");
    poweroff();

    kprint("Force panic falled. Power off failed. sleep until interupt. \n");
    while(1) {
        __asm__ volatile("wfi");
    }
}

void handle_trap() {
    kprint("\n!!! HARDWARE EXCEPTION DETECTED !!!\n");
    
    // Read the 'mcause' register to see WHY we trapped
    unsigned long cause;
    __asm__ volatile("csrr %0, mcause" : "=r"(cause));

    switch(cause) {
        case 0: kprint("Reason: Instruction Address Misaligned\n"); break;
        case 1: kprint("Reason: Instruction Access Fault\n"); break;
        case 2: kprint("Reason: Illegal Instruction\n"); break;
        case 3: kprint("Reason: Breakpoint (ebreak)\n"); break;
        case 4: kprint("Reason: Load Address Misaligned\n"); break;
        case 5: kprint("Reason: Load Access Fault\n"); break;
        case 6: kprint("Reason: Store/AMO Address Misaligned\n"); break;
        case 7: kprint("Reason: Store/AMO Access Fault\n"); break;
        default:
            kprintf("Reason: Unknown Exception Code %d\n", cause);
    }

    poweroff();
}