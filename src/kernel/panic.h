#ifndef PANIC_H
#define PANIC_H

void kpanic(const char *reason);
void kpanic_force();

#define KASSERT(cond, msg) \
    if (!(cond)) { \
        kprint("ASSERTION FAILED: "); \
        kprint(__FILE__); \
        kprint(":"); \
        /* Note: Printing line numbers requires a custom itoa/printf */ \
        kpanic(msg); \
    }

#endif