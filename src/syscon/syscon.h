#ifndef SYSCON_H
#define SYSCON_H

// QEMU Sifive Test device address
#define SYSCON_ADDR 0x100000

// Magic values for the Sifive Test device
#define SYSCON_POWEROFF 0x5555
#define SYSCON_REBOOT 0x7777

#define write_register(register, value) \
    asm volatile("csrrw zero, %0, %1" ::"i"(register), "r"(value))

#define read_register(register, destination) \
    asm volatile("csrrs %0, %1, zero" : "=r"(destination) : "i"(register))

#define MSTATUS 0x300
#define MIE 0x304
#define MSTATUS_BIT_MIE 3
#define MIE_BIT_MTIE 7
#define MIE_BIT_MEIE 11

void poweroff(void);
void reboot(void);

#endif