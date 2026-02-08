#include <stdint.h>
#include <stddef.h>
#include <drivers/uart.h>
#include <syscon/syscon.h>
#include <kernel/panic.h>
#include <kernel/memory.h>

void kmain() {
    kputs("Hello, OS World!");
    kputs("-------------------");
    knewline();
    page_init();
    poweroff();
}
