/* cpu/irq.c - Hardware IRQ dispatch + 8259 PIC remap */

#include "irq.h"
#include "idt.h"

/* 8259 PIC ports */
#define PIC1_CMD 0x20
#define PIC1_DATA 0x21
#define PIC2_CMD 0xA0
#define PIC2_DATA 0xA1
#define PIC_EOI 0x20

extern void irq0(void);
extern void irq1(void);
extern void irq2(void);
extern void irq3(void);
extern void irq4(void);
extern void irq5(void);
extern void irq6(void);
extern void irq7(void);
extern void irq8(void);
extern void irq9(void);
extern void irq10(void);
extern void irq11(void);
extern void irq12(void);
extern void irq13(void);
extern void irq14(void);
extern void irq15(void);

static irq_handler_t handlers[16] = {0};

void irq_register_handler(uint8_t irq, irq_handler_t h)
{
       if (irq < ARRAY_SIZE(handlers))
              handlers[irq] = h;
}

static void pic_remap(void)
{
       outb(PIC1_CMD, 0x11);
       io_wait();
       outb(PIC2_CMD, 0x11);
       io_wait();

       outb(PIC1_DATA, 0x20);
       io_wait();
       outb(PIC2_DATA, 0x28);
       io_wait();

       outb(PIC1_DATA, 0x04);
       io_wait();
       outb(PIC2_DATA, 0x02);
       io_wait();

       outb(PIC1_DATA, 0x01);
       io_wait();
       outb(PIC2_DATA, 0x01);
       io_wait();

       outb(PIC1_DATA, 0x00);
       outb(PIC2_DATA, 0x00);
}

void irq_init(void)
{
       pic_remap();

       void (*stubs[])(void) = {
           irq0, irq1, irq2, irq3, irq4, irq5, irq6, irq7,
           irq8, irq9, irq10, irq11, irq12, irq13, irq14, irq15};
       for (int i = 0; i < 16; i++)
              idt_set_gate((uint8_t)(32 + i), (uint32_t)stubs[i], 0x08, 0x8E);
}

/* Called by irq_common_stub in kernel_entry.asm */
void irq_handler(registers_t *r)
{
       uint8_t irq = (uint8_t)(r->int_no - 32);

       if (irq < ARRAY_SIZE(handlers) && handlers[irq])
              handlers[irq](r);

       if (irq < 16)
       {
              if (irq >= 8)
                     outb(PIC2_CMD, PIC_EOI);
              outb(PIC1_CMD, PIC_EOI);
       }
}
