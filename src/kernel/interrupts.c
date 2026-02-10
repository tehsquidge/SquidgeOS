#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include <drivers/uart.h>
#include <syscon/syscon.h>
#include <kernel/plic.h>
#include <kernel/interrupts.h>
#include <kernel/memory.h>

void interrupt_init()
{
    kprint("Initialising Interrupts...");
    uint64_t mstatus_val;
    asm volatile("csrr %0, mstatus" : "=r"(mstatus_val));
    mstatus_val |= (1 << MSTATUS_BIT_MIE);
    asm volatile("csrw mstatus, %0" ::"r"(mstatus_val));

    uint64_t mie_val;
    asm volatile("csrr %0, mie" : "=r"(mie_val));
    mie_val |= (1 << MIE_BIT_MEIE); //|(1 << MIE_BIT_MTIE);
    asm volatile("csrw mie, %0" ::"r"(mie_val));
    kputs("OK");
}

void kpanic(const char *reason, ...)
{
    va_list args;
	va_start(args, reason);
    kputs("\n!!! PANIC !!!\n");
    kprintf_internal(reason, args);
    kputs("\n!!! PANIC !!!\n");
    va_end(args);
    poweroff();
}

void kpanic_force()
{
    kprint("\n!!! FORCE PANIC !!!\n");

    // 'ebreak' is the standard RISC-V way to trigger a debug trap.
    __asm__ volatile("ebreak");

    kprint("Force panic falled. Power off. \n");
    poweroff();

    kprint("Force panic falled. Power off failed. sleep until interupt. \n");
    while (1)
    {
        __asm__ volatile("wfi");
    }
}

void handle_trap()
{
    // Read the 'mcause' register to see WHY we trapped
    unsigned long cause;
    __asm__ volatile("csrr %0, mcause" : "=r"(cause));

    // Check if the top bit is 1 (Interrupt) or 0 (Exception)
    // For 64-bit RISC-V, the bit is 63
    int is_interrupt = (cause >> 63) & 1;

    if (is_interrupt)
    {
        unsigned long code = cause & 0xfff;
        handle_interrupt(code);
        return;
    }
    else
    {
        // fault address (if applicable)
        uintptr_t mtval;
        asm volatile("csrr %0, mtval" : "=r"(mtval));

        switch (cause)
        {
        case 0:
            kpanic("Reason: Instruction Address Misaligned\n");
            break;
        case 1:
            kpanic("Reason: Instruction Access Fault\n");
            break;
        case 2:
            kpanic("Reason: Illegal Instruction\n");
            break;
        case 3:
            kpanic("Reason: Breakpoint (ebreak) %s\n", "woo!");
            break;
        case 4:
            kpanic("Reason: Load Address Misaligned\n");
            break;
        case 5:
            kpanic("Reason: Load Access Fault\n");
            break;
        case 6:
            kpanic("Reason: Store/AMO Address Misaligned\n");
            break;
        case 7:
            kpanic("Reason: Store/AMO Access Fault\n");
            break;
        default:
            break;
            kpanic("Reason: Unknown Exception Code %d\n", cause);
        }
        kprintf("Faulting Address (if applicable): %x\n", mtval);
    }
}

void handle_interrupt(unsigned long code)
{
    switch (code)
    {
    case 7:
        break; // timer Interrupt. Ignoring for now.
    case 11:
        volatile uint32_t *claim_reg = (uint32_t *)PLIC_CLAIM_COMPLETE;
        uint32_t irq = *claim_reg;
        if (irq == 10)
        {
            uart_handle_interrupt();
        }
        *claim_reg = irq;
        break;
    }
}