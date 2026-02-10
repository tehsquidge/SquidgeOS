#include <stdint.h>
#include "kernel/plic.h"

void plic_init()
{
    int hart = 0;

    // 1. Set the priority of the UART interrupt
    // We set it to 1. If it's 0 (the default), the interrupt is effectively disabled.
    *PLIC_PRIORITY(UART_IRQ) = 1;

    // 2. Enable the UART interrupt for Hart 0
    // This is a bitmask, so we shift 1 by the IRQ number.
    *PLIC_ENABLE(hart) = (1 << UART_IRQ);

    // 3. Set the priority threshold for Hart 0
    // We set this to 0 so that ANY interrupt with priority > 0 gets through.
    *PLIC_THRESHOLD(hart) = 0;
}