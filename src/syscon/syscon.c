#include <stdint.h>
#include <stddef.h>
#include "syscon.h"
#include "drivers/uart.h"

void poweroff(void) {
  	kputs("Poweroff requested");
	*(volatile uint32_t *)SYSCON_ADDR = SYSCON_POWEROFF;
}

void reboot(void) {
  	kputs("Reboot requested");
	*(volatile uint32_t *)SYSCON_ADDR = SYSCON_REBOOT;
}