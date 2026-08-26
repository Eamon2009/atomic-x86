CC      = i686-elf-gcc
CXX     = i686-elf-g++
AS      = nasm
LD      = i686-elf-ld
CFLAGS  = -m32 -ffreestanding -O2 -Wall -Wextra \
          -fno-stack-protector -nostdlib -nostdinc \
          -I kernel -I lib
CXXFLAGS = -m32 -ffreestanding -O2 -Wall -Wextra \
           -fno-stack-protector -fno-exceptions -fno-rtti \
           -fno-threadsafe-statics -fno-use-cxa-atexit \
           -nostdlib -nostdinc -nostdinc++ \
           -I kernel -I lib
ASFLAGS = -f elf32
LDFLAGS = -T linker.ld -m elf_i386 --nostdlib
TARGET  = mykernel.bin
ISO     = mykernel.iso
C_SRCS :=
C_SRCS += kernel/main.c
C_SRCS += kernel/panic.c
C_SRCS += kernel/shell.c
C_SRCS += kernel/cpp_runtime.c
C_SRCS += drivers/vga.c
C_SRCS += drivers/keyboard.c
C_SRCS += drivers/timer.c
C_SRCS += cpu/gdt.c
C_SRCS += cpu/idt.c
C_SRCS += cpu/isr.c
C_SRCS += cpu/irq.c
C_SRCS += mm/kmalloc.c
C_SRCS += mm/paging.c
C_SRCS += mm/pmm.c
C_SRCS += lib/string.c
C_SRCS += lib/kprintf.c
C_SRCS += lib/ports.c
ASM_SRCS :=
ASM_SRCS += boot/boot.asm
ASM_SRCS += boot/kernel_entry.asm
ASM_SRCS += cpu/tables_flush.asm
CPP_SRCS :=
C_OBJS   = $(C_SRCS:.c=.o)
CPP_OBJS = $(CPP_SRCS:.cpp=.o)
ASM_OBJS = $(ASM_SRCS:.asm=.o)
OBJS     = $(ASM_OBJS) $(C_OBJS) $(CPP_OBJS)
.PHONY: all clean iso run
all: $(TARGET)
$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Compile C++
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
%.o: %.asm
	$(AS) $(ASFLAGS) $< -o $@
iso: $(TARGET)
	mkdir -p isodir/boot/grub
	cp $(TARGET) isodir/boot/$(TARGET)
	cp grub.cfg isodir/boot/grub/grub.cfg
	grub-mkrescue -o $(ISO) isodir
run: iso
	qemu-system-i386 -cdrom $(ISO)

clean:
	rm -f $(OBJS) $(TARGET) $(ISO)
	rm -rf isodir
