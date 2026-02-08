#include <stdint.h>
#include <stddef.h>
#include <drivers/uart.h>
#include <syscon/syscon.h>

void kmain() {
    kprint("Hello, OS World!\n");
    poweroff();
}
