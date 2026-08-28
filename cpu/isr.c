/* cpu/isr.c */

#include "isr.h"
#include "idt.h"
#include "kernel/panic.h"
#include "lib/kprintf.h"
extern void isr0(void);
extern void isr1(void);
extern void isr2(void);
extern void isr3(void);
extern void isr4(void);
extern void isr5(void);
extern void isr6(void);
extern void isr7(void);
extern void isr8(void);
extern void isr9(void);
extern void isr10(void);
extern void isr11(void);
extern void isr12(void);
extern void isr13(void);
extern void isr14(void);
extern void isr15(void);
extern void isr16(void);
extern void isr17(void);
extern void isr18(void);
extern void isr19(void);
extern void isr20(void);
extern void isr21(void);
extern void isr22(void);
extern void isr23(void);
extern void isr24(void);
extern void isr25(void);
extern void isr26(void);
extern void isr27(void);
extern void isr28(void);
extern void isr29(void);
extern void isr30(void);
extern void isr31(void);

static const char *exception_names[] = {
    "Division by zero", "Debug",
    "NMI", "Breakpoint",
    "Overflow", "Bound range exceeded",
    "Invalid opcode", "Device not available",
    "Double fault", "Coprocessor segment overrun",
    "Invalid TSS", "Segment not present",
    "Stack-segment fault", "General protection fault",
    "Page fault", "Reserved",
    "x87 FPU error", "Alignment check",
    "Machine check", "SIMD FPU exception",
    "Virtualisation fault", "Reserved",
    "Reserved", "Reserved", "Reserved", "Reserved",
    "Reserved", "Reserved", "Reserved", "Reserved",
    "Security exception", "Reserved"};

static isr_t handlers[32] = {0};

void isr_register_handler(uint8_t num, isr_t handler)
{
       if (num < ARRAY_SIZE(handlers))
              handlers[num] = handler;
}

void register_interrupt_handler(uint8_t n, isr_t handler)
{
       isr_register_handler(n, handler);
}

void isr_init(void)
{
       void (*stubs[])(void) = {
           isr0, isr1, isr2, isr3, isr4, isr5, isr6, isr7,
           isr8, isr9, isr10, isr11, isr12, isr13, isr14, isr15,
           isr16, isr17, isr18, isr19, isr20, isr21, isr22, isr23,
           isr24, isr25, isr26, isr27, isr28, isr29, isr30, isr31};
       for (int i = 0; i < 32; i++)
              idt_set_gate((uint8_t)i, (uint32_t)stubs[i], 0x08, 0x8E);
}

/* Called by isr_common_stub in kernel_entry.asm */
void isr_handler(registers_t *r)
{
       if (r->int_no < ARRAY_SIZE(handlers) && handlers[r->int_no])
       {
              handlers[r->int_no](r);
              return;
       }

       panic_set_regs(r);
       if (r->int_no < ARRAY_SIZE(exception_names))
       {
              kprintf("[EXCEPTION] %s (int %u, err 0x%x)\n",
                      exception_names[r->int_no], r->int_no, r->err_code);
              panic(exception_names[r->int_no]);
       }
       kprintf("[EXCEPTION] Unknown exception (int %u, err 0x%x)\n",
               r->int_no, r->err_code);
       panic("Unknown CPU exception");
}
