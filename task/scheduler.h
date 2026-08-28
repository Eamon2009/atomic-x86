#ifndef SCHEDULER_H
#define SCHEDULER_H
#include "kernel/kernel.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct task
    {
        uint32_t id;
        uint32_t esp;
        uint32_t ebp;
        uint32_t eip;
        uint32_t cr3;
        uint32_t state; /* 0 = ready, 1 = running, 2 = blocked */
        struct task *next;
    } task_t;

    void scheduler_init(void);
    void scheduler_add_task(task_t *task);
    void scheduler_remove_task(uint32_t id);
    task_t *scheduler_current(void);
    void scheduler_switch_task(registers_t *r);

#ifdef __cplusplus
}
#endif
#endif