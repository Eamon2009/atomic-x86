/* kernel/main.cpp */

#include "shell.h"
#include "cpp_runtime.h"
#include "kernel.h"

/* drivers */
#include "../drivers/vga.h"
#include "../drivers/keyboard.h"
#include "../drivers/timer.h"

/* cpu */
#include "../cpu/gdt.h"
#include "../cpu/idt.h"
#include "../cpu/isr.h"
#include "../cpu/irq.h"

/* memory */
#include "../mm/pmm.h"
#include "../mm/paging.h"
#include "../mm/kmalloc.h"

/* lib */
#include "../lib/kprintf.h"

extern "C" void kernel_main(uint32_t magic, multiboot_info_t *mbi)
{
       // Initialize VGA text output
       vga_init();

       kprintf("mini-kernel booting...\n");

       // Check multiboot magic
       if (magic != MULTIBOOT_MAGIC)
       {
              kprintf("[FATAL] Bad multiboot magic: 0x%x\n", magic);
              panic("Invalid bootloader");
       }
       kprintf("[OK] Multiboot magic verified\n");

       // Initialize optional C++ runtime (constructors)
       cpp_init();
       kprintf("[OK] C++ runtime init complete\n");

       // CPU setup
       gdt_init();
       kprintf("[OK] GDT loaded\n");

       idt_init();
       kprintf("[OK] IDT loaded\n");

       isr_init();
       kprintf("[OK] ISRs installed\n");

       irq_init();
       kprintf("[OK] IRQs remapped and installed\n");

       // Memory setup
       pmm_init(mbi);
       kprintf("[OK] PMM initialised\n");

       paging_init();
       kprintf("[OK] Paging enabled\n");

       kmalloc_init();
       kprintf("[OK] Heap ready\n");

       // Device drivers
       timer_init(100);
       kprintf("[OK] Timer at 100 Hz\n");

       keyboard_init();
       kprintf("[OK] Keyboard driver ready\n");

       // Enable interrupts
       asm volatile("sti");
       kprintf("[OK] Interrupts enabled\n");

       kprintf("\nmini-kernel ready.\n\n");

       // Start shell
       shell_run();

       // Halt CPU when shell exits
       while (true)
       {
              asm volatile("hlt");
       }
}