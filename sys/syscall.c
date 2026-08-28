#include "sys/syscall.h"
#include "cpu/idt.h"
#include "cpu/irq.h"
#include "drivers/vga.h"
#include "drivers/timer.h"
#include "lib/kprintf.h"
#include "task/scheduler.h"

static uint32_t sys_write(registers_t *r)
{
    const char *buf = (const char *)r->ebx;
    uint32_t len = r->ecx;
    for (uint32_t i = 0; i < len; i++)
        vga_putchar(buf[i]);
    return len;
}

static uint32_t sys_exit(registers_t *r)
{
    kprintf("[SYSCALL] exit(%u)\n", r->ebx);
    scheduler_remove_task(scheduler_current()->id);
    for (;;)
        __asm__ __volatile__("hlt");
    return 0;
}

static uint32_t sys_getpid(registers_t *r)
{
    UNUSED(r);
    task_t *t = scheduler_current();
    return t ? t->id : 0;
}

static uint32_t sys_sleep(registers_t *r)
{
    timer_sleep(r->ebx);
    return 0;
}

void syscall_init(void)
{
    /*
     * 0xEE = present | DPL=3 | 32-bit interrupt gate */
    idt_set_gate(0x80, (uint32_t)syscall_interrupt, 0x08, 0xEE);
    kprintf("[SYSCALL] int 0x80 installed (ring 3 callable)\n");
}

void syscall_handler(registers_t *r)
{
    static uint32_t (*syscalls[16])(registers_t *) = {
        [SYSCALL_EXIT] = sys_exit,
        [SYSCALL_WRITE] = sys_write,
        [SYSCALL_GETPID] = sys_getpid,
        [SYSCALL_SLEEP] = sys_sleep,
    };

    if (r->eax < 16 && syscalls[r->eax])
        r->eax = syscalls[r->eax](r);
    else
    {
        kprintf("[SYSCALL] unknown syscall %u\n", r->eax);
        r->eax = (uint32_t)-1;
    }
}