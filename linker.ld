
ENTRY(kernel_start)         

SECTIONS
{
    . = 0x100000;          
    .text ALIGN(4K) :
    {
        *(.multiboot)       
        *(.text)           
    }
    .rodata ALIGN(4K) :
    {
        *(.rodata)
        *(.rodata.*)
    }
    .init_array ALIGN(4K) :
    {
        __init_array_start = .;
        KEEP(*(.init_array))
        KEEP(*(.init_array.*))
        __init_array_end = .;
    }
    .data ALIGN(4K) :
    {
        *(.data)
    }
    .bss ALIGN(4K) :
    {
        *(COMMON)
        *(.bss)
        *(.bss.*)
    }
    kernel_end = .;
    /DISCARD/ :
    {
        *(.comment)
        *(.eh_frame)
        *(.note.GNU-stack)
    }
}
