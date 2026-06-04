/* cpu/gdt.h */
#ifndef GDT_H
#define GDT_H
#include "../kernel/kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
       uint16_t limit_low;
       uint16_t base_low;
       uint8_t base_mid;
       uint8_t access; /* P|DPL|S|Type */
       uint8_t gran;   /* G|DB|L|AVL|limit_high */
       uint8_t base_high;
} PACKED gdt_entry_t;

typedef struct
{
       uint16_t limit;
       uint32_t base;
} PACKED gdt_ptr_t;

void gdt_init(void);

#ifdef __cplusplus
}
#endif
#endif
