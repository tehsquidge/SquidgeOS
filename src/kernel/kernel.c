#include <stdint.h>
#include <stddef.h>
#include <drivers/uart.h>
#include <syscon/syscon.h>
#include <kernel/plic.h>
#include <kernel/interrupts.h>
#include <kernel/memory.h>

void kmain()
{
    zero_bss();
    plic_init();
    uart_init();
    page_init();
    interrupt_init();
    kputs("----------------------");
    kprintf("Hello, from %s!", "SquidgeOS");
    kputs("----------------------");
    knewline();
}
