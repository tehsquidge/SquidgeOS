#ifndef PANIC_H
#define PANIC_H

#define MSTATUS 0x300
#define MIE 0x304
#define MSTATUS_BIT_MIE 3
#define MIE_BIT_MTIE 7
#define MIE_BIT_MEIE 11

#define CLINT_BASE 0x02000000UL
#define CLINT_MTIME     (CLINT_BASE + 0xBFF8UL)
#define CLINT_MTIMECMP(h)   (CLINT_BASE + 0x4000UL + (8UL * (h)))


// This should match the order of registers in traps.S
typedef struct {
    // Return address and pointers
    uint64_t ra;      // x1
    uint64_t gp;      // x3
    uint64_t tp;      // x4

    // Temporary registers
    uint64_t t0;      // x5
    uint64_t t1;      // x6
    uint64_t t2;      // x7

    // Saved registers
    uint64_t s0;      // x8 (fp)
    uint64_t s1;      // x9

    // Function arguments / Return values
    uint64_t a0;      // x10
    uint64_t a1;      // x11
    uint64_t a2;      // x12
    uint64_t a3;      // x13
    uint64_t a4;      // x14
    uint64_t a5;      // x15
    uint64_t a6;      // x16
    uint64_t a7;      // x17

    // More saved registers
    uint64_t s2;      // x18
    uint64_t s3;      // x19
    uint64_t s4;      // x20
    uint64_t s5;      // x21
    uint64_t s6;      // x22
    uint64_t s7;      // x23
    uint64_t s8;      // x24
    uint64_t s9;      // x25
    uint64_t s10;     // x26
    uint64_t s11;     // x27

    // More temporary registers
    uint64_t t3;      // x28
    uint64_t t4;      // x29
    uint64_t t5;      // x30
    uint64_t t6;      // x31

    // Control and Status Register state
    uint64_t mepc;    // offset 240 (Saved in traps.S)
} trap_frame_t;

void interrupt_init();
void kpanic(const char *, ...);
void kpanic_force();
void handle_trap(trap_frame_t *registers);
void handle_interrupt(unsigned long code);

#define KASSERT(cond, msg)                                              \
    if (!(cond))                                                        \
    {                                                                   \
        kprint("ASSERTION FAILED: ");                                   \
        kprint(__FILE__);                                               \
        kprint(":");                                                    \
        /* Note: Printing line numbers requires a custom itoa/printf */ \
        kpanic(msg);                                                    \
    }

#endif