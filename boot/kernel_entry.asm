; boot/kernel_entry.asm
; Secondary entry helpers ISR/IRQ low-level stubs and
; the isr_common / irq_common trampolines that save CPU
; state before calling into C handlers.
;
; boot.asm already set up the stack and called kernel_main.
; This file provides the assembly side of the interrupt
; dispatch that cpu/isr.c and cpu/irq.c depend on.

bits 32
extern isr_handler          ; cpu/isr.c
extern irq_handler          ; cpu/irq.c

; Common ISR trampoline 
isr_common_stub:
    pusha                   ; push eax,ecx,edx,ebx,esp,ebp,esi,edi
    mov  ax, ds
    push eax                ; save data segment

    mov  ax, 0x10           ; kernel data segment descriptor
    mov  ds, ax
    mov  es, ax
    mov  fs, ax
    mov  gs, ax

    push esp                ; arg: pointer to register frame
    call isr_handler        ;cpu/isr.c
    add  esp, 4

    pop  eax                ; restore saved data segment
    mov  ds, ax
    mov  es, ax
    mov  fs, ax
    mov  gs, ax

    popa
    add  esp, 8            
    iret                  

irq_common_stub:
    pusha
    mov  ax, ds
    push eax

    mov  ax, 0x10
    mov  ds, ax
    mov  es, ax
    mov  fs, ax
    mov  gs, ax

    push esp
    call irq_handler        ; cpu/irq.c
    add  esp, 4

    pop  eax
    mov  ds, ax
    mov  es, ax
    mov  fs, ax
    mov  gs, ax

    popa
    add  esp, 8
    iret
%macro ISR_NOERR 1
global isr%1
isr%1:
    push byte 0             ; dummy error code
    push byte %1            ; interrupt number
    jmp  isr_common_stub
%endmacro

%macro ISR_ERR 1
global isr%1
isr%1:
    push byte %1            ; CPU already pushed error code
    jmp  isr_common_stub
%endmacro

ISR_NOERR  0   ; divide by zero
ISR_NOERR  1   ; debug
ISR_NOERR  2   ; NMI
ISR_NOERR  3   ; breakpoint
ISR_NOERR  4   ; overflow
ISR_NOERR  5   ; bound range exceeded
ISR_NOERR  6   ; invalid opcode
ISR_NOERR  7   ; device not available
ISR_ERR    8   ; double fault
ISR_NOERR  9
ISR_ERR   10   ; invalid TSS
ISR_ERR   11   ; segment not present
ISR_ERR   12   ; stack-segment fault
ISR_ERR   13   ; general protection fault
ISR_ERR   14   ; page fault
ISR_NOERR 15
ISR_NOERR 16   ; x87 FPU error
ISR_ERR   17   ; alignment check
ISR_NOERR 18   ; machine check
ISR_NOERR 19   ; SIMD FPU exception
ISR_NOERR 20
ISR_NOERR 21
ISR_NOERR 22
ISR_NOERR 23
ISR_NOERR 24
ISR_NOERR 25
ISR_NOERR 26
ISR_NOERR 27
ISR_NOERR 28
ISR_NOERR 29
ISR_ERR   30   ; security exception
ISR_NOERR 31

%macro IRQ 2
global irq%1
irq%1:
    push byte 0
    push byte %2            ; 
    jmp  irq_common_stub
%endmacro

IRQ  0, 32   ; PIT timer
IRQ  1, 33   ; PS/2 keyboard
IRQ  2, 34
IRQ  3, 35
IRQ  4, 36
IRQ  5, 37
IRQ  6, 38
IRQ  7, 39
IRQ  8, 40   ; RTC
IRQ  9, 41
IRQ 10, 42
IRQ 11, 43
IRQ 12, 44   ; PS/2 mouse
IRQ 13, 45   ; FPU
IRQ 14, 46   ; primary ATA
IRQ 15, 47   ; secondary ATA