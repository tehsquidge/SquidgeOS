#include <stdint.h>
#include <stddef.h>
#include <drivers/uart.h>
#include <syscon/syscon.h>
#include <kernel/panic.h>
#include <kernel/memory.h>

void kmain() {
    kprintf("Hello, from %s v%f!" , "SquidgeOS", 0.5);
    kputs("-------------------");
    knewline();
    page_init();

    //kprint("Stress testing memory...\n");
    //while(1) {
    //    void *p = page_alloc();
    //}

    test_memory_integrity();

    poweroff();
}
