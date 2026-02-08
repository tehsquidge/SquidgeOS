#include <stdint.h>
#include <stddef.h>
#include <drivers/uart.h>
#include <syscon/syscon.h>

void handle_trap() {
    kprint("\n!!! HARDWARE EXCEPTION DETECTED !!!\n");
    
    // Read the 'mcause' register to see WHY we trapped
    unsigned long cause;
    __asm__ volatile("csrr %0, mcause" : "=r"(cause));

    if (cause == 3) { // 3 is the code for 'breakpoint' (ebreak)
        kprint("Reason: ebreak (Breakpoint)\n");
    } else {
        kprint("Reason: Other Exception\n");
    }

    poweroff();
}