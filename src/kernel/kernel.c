#include <stdint.h>
#include <stddef.h>
#include <drivers/uart.h>
#include <syscon/syscon.h>
#include <kernel/panic.h>

void kmain() {
    kprint("Hello, OS World!\n");
    kpanic_force();
    poweroff();
}
