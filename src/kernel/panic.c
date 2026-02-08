#include <stddef.h>
#include <stdint.h>
#include <drivers/uart.h>
#include <syscon/syscon.h>

void kpanic(const char *reason){
	kprint(reason);
	poweroff();
}

void kpanic_force() {
    // 1. Immediate Output
    kprint("\n!!! FORCE PANIC !!!\n");

    // 2. Trigger a hardware breakpoint or illegal instruction
    // This allows a debugger (GDB) to stop exactly here.
    // 'ebreak' is the standard RISC-V way to trigger a debug trap.
    __asm__ volatile("ebreak");

    // 3. If no debugger is attached or we continue, kill the VM
    poweroff();

    // 4. Final halt
    while(1) {
        __asm__ volatile("wfi");
    }
}
