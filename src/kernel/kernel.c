#include <stdint.h>
#include <stddef.h>
#include <drivers/uart.h>
#include <syscon/syscon.h>
#include <kernel/interrupts.h>
#include <kernel/memory.h>

void kmain()
{
    kprintf("Hello, from %s!", "SquidgeOS");
    kputs("-------------------");
    knewline();
    page_init();
    test_memory_integrity();
    test_memory_alignment();
    test_memory_stress();
    poweroff();
}
