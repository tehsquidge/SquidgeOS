#ifndef SYSCON_H
#define SYSCON_H

// QEMU Sifive Test device address
#define SYSCON_ADDR 0x100000

// Magic values for the Sifive Test device
#define SYSCON_POWEROFF 0x5555
#define SYSCON_REBOOT 0x7777

void poweroff(void);
void reboot(void);

#endif