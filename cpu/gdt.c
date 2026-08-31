/* cpu/gdt.c — Global Descriptor Table
 * Flat memory model:
 *   0x00 null descriptor
 *   0x08 kernel code  (ring 0, executable, readable)
 *   0x10 kernel data  (ring 0, writable)
 * All segments span the full 4 GB address space.
 *
 * CRITICAL: gdt_flush.asm must perform:
 *   1. lgdt [gdt_ptr]
 *   2. Far jump to 0x08: to reload CS
 *   3. Reload DS/ES/FS/GS/SS with 0x10
 */

#include "gdt.h"

#define GDT_ENTRIES 3

static gdt_entry_t gdt[GDT_ENTRIES];
static gdt_ptr_t gdt_ptr;

extern void gdt_flush(uint32_t); /* in gdt_flush.asm */

static void gdt_set(int i, uint32_t base, uint32_t limit,
                    uint8_t access, uint8_t gran)
{

       if (i < 0 || i >= GDT_ENTRIES)
              return;

       gdt[i].base_low = base & 0xFFFF;
       gdt[i].base_mid = (base >> 16) & 0xFF;
       gdt[i].base_high = (base >> 24) & 0xFF;
       gdt[i].limit_low = limit & 0xFFFF;
       gdt[i].gran = (gran & 0xF0) | ((limit >> 16) & 0x0F);
       gdt[i].access = access;
}

void gdt_init(void)
{
       gdt_ptr.limit = sizeof(gdt) - 1;
       gdt_ptr.base = (uint32_t)&gdt;

       gdt_set(0, 0, 0, 0x00, 0x00);          /* null */
       gdt_set(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); /* 0x08 kernel code */
       gdt_set(2, 0, 0xFFFFFFFF, 0x92, 0xCF); /* 0x10 kernel data */

       gdt_flush((uint32_t)&gdt_ptr);
       __asm__ volatile(
           "movw $0x10, %%ax\n\t"
           "movw %%ax, %%ds\n\t"
           "movw %%ax, %%es\n\t"
           "movw %%ax, %%fs\n\t"
           "movw %%ax, %%gs\n\t"
           "movw %%ax, %%ss\n\t" ::: "ax");
}