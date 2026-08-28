#ifndef ASSERT_H
#define ASSERT_H
#include "kernel/panic.h"
#include "lib/kprintf.h"

#define ASSERT(cond)                                                    \
    do {                                                                \
        if (!(cond)) {                                                  \
            kprintf("[ASSERT] %s:%u: %s\n", __FILE__, __LINE__, #cond); \
            panic("Assertion failed");                                  \
        }                                                               \
    } while (0)

#define ASSERT_MSG(cond, msg)                                         \
    do {                                                              \
        if (!(cond)) {                                                \
            kprintf("[ASSERT] %s:%u: %s\n", __FILE__, __LINE__, msg); \
            panic(msg);                                               \
        }                                                             \
    } while (0)

#endif