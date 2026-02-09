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
