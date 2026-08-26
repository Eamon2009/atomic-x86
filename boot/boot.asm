
bits 32
section .multiboot
MULTIBOOT_MAGIC    equ 0x1BADB002
MULTIBOOT_FLAGS    equ 0x00000003  
MULTIBOOT_CHECKSUM equ -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)

align 4
    dd MULTIBOOT_MAGIC
    dd MULTIBOOT_FLAGS
    dd MULTIBOOT_CHECKSUM
section .bss
align 16
stack_bottom:
    resb 16384              ; 16 KB stack
stack_top:
section .text
global kernel_start

kernel_start:
    mov  esp, stack_top
    push ebx                
    push eax               
    call kernel_main
.hang:
    cli                     ; disable interrupts
    hlt                     ; halt until next interrupt (none coming)
    jmp .hang               ; paranoia: loop if NMI wakes us
