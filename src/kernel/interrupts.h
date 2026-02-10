#ifndef PANIC_H
#define PANIC_H

void interrupt_init();
void kpanic(const char *);
void kpanic_force();
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