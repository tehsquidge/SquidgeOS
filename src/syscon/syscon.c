#include <stdint.h>
#include <stddef.h>
#include "syscon.h"
#include "drivers/uart.h"

void poweroff(void) {
  kputs("Poweroff requested");
  *(uint32_t *)SYSCON_ADDR = 0x5555;
}

void reboot(void) {
  kputs("Reboot requested");
  *(uint32_t *)SYSCON_ADDR = 0x7777;
}