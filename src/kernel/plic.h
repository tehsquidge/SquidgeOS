#ifndef PLIC_H
#define PLIC_H

#define PLIC_BASE 0x0c000000
#define PLIC_CLAIM_COMPLETE 0x0c200004

// Priorities: 4 bytes per IRQ (IRQ 0 is reserved/null)
#define PLIC_PRIORITY(irq) ((volatile uint32_t *)(PLIC_BASE + (irq) * 4))

// Enables: Each Hart has a 0x80 byte stride for its enable bits
// For Hart 0 Machine Mode: 0x0c002000
#define PLIC_ENABLE(hart) ((volatile uint32_t *)(PLIC_BASE + 0x2000 + (hart) * 0x80))

// Threshold and Claim/Complete: Each Hart has a 0x1000 byte stride
// For Hart 0 Machine Mode: 0x0c200000 and 0x0c200004
#define PLIC_THRESHOLD(hart) ((volatile uint32_t *)(PLIC_BASE + 0x200000 + (hart) * 0x1000))
#define PLIC_CLAIM(hart) ((volatile uint32_t *)(PLIC_BASE + 0x200004 + (hart) * 0x1000))

#define UART_IRQ 10

void plic_init();

#endif