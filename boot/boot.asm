; boot/boot.asm
; GRUB multiboot header + minimal CPU setup before C entry

bits 32
section .multiboot

; ── Multiboot 1 header ─────────────────────────────────────
; GRUB scans the first 8 KB of the binary for this magic.
; Must be 4-byte aligned.
MULTIBOOT_MAGIC    equ 0x1BADB002
MULTIBOOT_FLAGS    equ 0x00000003   ; bit0=page-align, bit1=give memory map
MULTIBOOT_CHECKSUM equ -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)

align 4
    dd MULTIBOOT_MAGIC
    dd MULTIBOOT_FLAGS
    dd MULTIBOOT_CHECKSUM

; ── Stack ──────────────────────────────────────────────────
; C needs a valid stack before it can do anything.
; We carve out 16 KB in BSS (zero-filled, no binary bloat).
section .bss
align 16
stack_bottom:
    resb 16384              ; 16 KB stack
stack_top:

; ── Entry point ────────────────────────────────────────────
; linker.ld ENTRY(kernel_start) points here.
section .text
global kernel_start

kernel_start:
    ; Point ESP to top of our stack (stack grows downward)
    mov  esp, stack_top

    ; EBX holds the multiboot info struct pointer from GRUB.
    ; Pass it to kernel_main as first argument (cdecl: push right-to-left).
    push ebx                ; arg1: multiboot_info_t*
    push eax                ; arg2: multiboot magic (0x2BADB002 if valid)

    ; Hand off to C
    call kernel_main

    ; kernel_main should never return.
    ; If it does, halt the CPU in an infinite loop.
.hang:
    cli                     ; disable interrupts
    hlt                     ; halt until next interrupt (none coming)
    jmp .hang               ; paranoia: loop if NMI wakes us