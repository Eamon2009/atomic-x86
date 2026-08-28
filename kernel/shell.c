/* kernel/shell.c
 * kernel shell
 *
 * use only what already exists in the kernel:
 *   keyboard_getchar()     - drivers/keyboard.h
 *   vga_putchar()          - drivers/vga.h
 *   vga_clear()            - drivers/vga.h
 *   vga_set_color()        - drivers/vga.h
 *   kprintf()              - lib/kprintf.h
 *   strcmp() / strlen()    - lib/string.h
 *   timer_get_ticks()      - drivers/timer.h
 *   pmm_free_count()       - mm/pmm.h
 *   PMM_FRAME_SIZE         - mm/pmm.h
 *   kmalloc() / kfree()    - mm/kmalloc.h
 *   registers_t            - kernel/kernel.h
 *   VGA_COLOR_*            - drivers/vga.h
 *   MULTIBOOT_MAGIC         kernel/kernel.h
 *
 */

#include "shell.h"
#include "kernel/kernel.h"
#include "drivers/keyboard.h"
#include "drivers/vga.h"
#include "drivers/timer.h"
#include "lib/kprintf.h"
#include "lib/string.h"
#include "mm/pmm.h"
#include "mm/kmalloc.h"
#define PROMPT "helium> "
static void cmd_help(void);
static void cmd_clear(void);
static void cmd_mem(void);
static void cmd_uptime(void);
static void cmd_echo(const char *args);
static void cmd_color(const char *args);
static void cmd_reboot(void);
static void cmd_version(void);
static void cmd_panic_test(void);

//----------------------------------------------
typedef struct
{
       const char *name;
       const char *description;
       void (*handler)(const char *args);
} command_t;
static void _help_wrap(const char *a)
{
       UNUSED(a);
       cmd_help();
}
static void _clear_wrap(const char *a)
{
       UNUSED(a);
       cmd_clear();
}
static void _mem_wrap(const char *a)
{
       UNUSED(a);
       cmd_mem();
}
static void _up_wrap(const char *a)
{
       UNUSED(a);
       cmd_uptime();
}
static void _reboot_wrap(const char *a)
{
       UNUSED(a);
       cmd_reboot();
}
static void _ver_wrap(const char *a)
{
       UNUSED(a);
       cmd_version();
}
static void _panic_wrap(const char *a)
{
       UNUSED(a);
       cmd_panic_test();
}

static const command_t commands[] = {
    {"help", "show this help message", _help_wrap},
    {"clear", "clear the screen", _clear_wrap},
    {"mem", "show free and total physical RAM", _mem_wrap},
    {"uptime", "show system uptime in seconds", _up_wrap},
    {"echo", "echo text back to screen", cmd_echo},
    {"color", "set text color  e.g. color green", cmd_color},
    {"version", "show kernel version information", _ver_wrap},
    {"reboot", "reboot the system via CPU reset", _reboot_wrap},
    {"panic_test", "trigger a test kernel panic", _panic_wrap},
};

#define CMD_COUNT ARRAY_SIZE(commands)
static void shell_putchar(char c)
{
       vga_putchar(c);
}
static void print_prompt(void)
{
       vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
       kprintf(PROMPT);
       vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
}
static void read_line(char *buf, size_t len)
{
       size_t pos = 0;

       while (1)
       {
              char c = keyboard_getchar();

              if (c == '\n' || c == '\r')
              {
                     shell_putchar('\n');
                     break;
              }

              if (c == '\b')
              {
                     if (pos > 0)
                     {
                            pos--;
                            shell_putchar('\b');
                            shell_putchar(' ');
                            shell_putchar('\b');
                     }
                     continue;
              }
              if (c >= 0x20 && c < 0x7F && pos < len - 1)
              {
                     buf[pos++] = c;
                     shell_putchar(c);
              }
       }

       buf[pos] = '\0';
}
static const char *trim_left(const char *s)
{
       while (*s == ' ')
              s++;
       return s;
}
static const char *find_space(const char *s)
{
       while (*s && *s != ' ')
              s++;
       return s;
}
static void cmd_help(void)
{
       kprintf("\n  Available commands:\n\n");
       for (size_t i = 0; i < CMD_COUNT; i++)
       {
              vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
              kprintf("    %-12s", commands[i].name);
              vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
              kprintf("  %s\n", commands[i].description);
       }
       kprintf("\n");
}

static void cmd_clear(void)
{
       vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
       vga_clear();
}

static void cmd_mem(void)
{
       uint32_t free_frames = pmm_free_count();
       uint32_t free_mb = (free_frames * PMM_FRAME_SIZE) >> 20;
       uint32_t free_kb = (free_frames * PMM_FRAME_SIZE) >> 10;

       kprintf("\n  Physical memory\n");
       kprintf("---------------\n");
       kprintf("Free frames : %u\n", free_frames);
       kprintf("Free memory : %u KB  (%u MB)\n\n", free_kb, free_mb);
}

static void cmd_uptime(void)
{
       uint32_t ticks = timer_get_ticks();
       uint32_t seconds = ticks / 100;
       uint32_t minutes = seconds / 60;
       uint32_t hours = minutes / 60;

       seconds %= 60;
       minutes %= 60;

       kprintf("\n  Uptime: %u:%u:%u  (h:m:s)  [%u ticks @ 100 Hz]\n\n",
               hours, minutes, seconds, ticks);
}

static void cmd_echo(const char *args)
{
       const char *text = trim_left(args);
       if (*text == '\0')
       {
              kprintf("\n usage: echo <text>\n\n");
              return;
       }
       kprintf("\n  %s\n\n", text);
}

static void cmd_color(const char *args)
{
       const char *color = trim_left(args);

       vga_color_t fg = VGA_COLOR_LIGHT_GREY; /* default */

       if (strcmp(color, "white") == 0)
              fg = VGA_COLOR_WHITE;
       else if (strcmp(color, "green") == 0)
              fg = VGA_COLOR_LIGHT_GREEN;
       else if (strcmp(color, "cyan") == 0)
              fg = VGA_COLOR_LIGHT_CYAN;
       else if (strcmp(color, "red") == 0)
              fg = VGA_COLOR_LIGHT_RED;
       else if (strcmp(color, "blue") == 0)
              fg = VGA_COLOR_LIGHT_BLUE;
       else if (strcmp(color, "magenta") == 0)
              fg = VGA_COLOR_LIGHT_MAGENTA;
       else if (strcmp(color, "brown") == 0)
              fg = VGA_COLOR_BROWN;
       else if (strcmp(color, "grey") == 0)
              fg = VGA_COLOR_LIGHT_GREY;
       else if (strcmp(color, "yellow") == 0)
              fg = VGA_COLOR_LIGHT_BROWN;
       else
       {
              kprintf("\n  unknown color: %s\n", color);
              kprintf("  options: white green cyan red blue magenta brown grey yellow\n\n");
              return;
       }

       vga_set_color(fg, VGA_COLOR_BLACK);
       kprintf("\n  Color set to %s\n\n", color);
}

static void cmd_version(void)
{
       kprintf("\n  mini-kernel  v0.1.0-alpha\n");
       kprintf("  arch    : i686 (32-bit protected mode)\n");
       kprintf("  boot    : GRUB Multiboot 1  (magic 0x%x)\n", MULTIBOOT_MAGIC);
       kprintf("  timer   : PIT 8253/8254 @ 100 Hz\n");
       kprintf("  build   : " __DATE__ "  " __TIME__ "\n\n");
}

static void cmd_reboot(void)
{
       kprintf("\n Rebooting...\n");
       uint8_t good = 0x02;
       while (good & 0x02)
              good = inb(0x64);
       outb(0x64, 0xFE);
       __asm__ __volatile__(
           "lidt %0\n"
           "int  $3\n" ::"m"((uint64_t){0}));
       for (;;)
              __asm__ __volatile__("hlt");
}

static void cmd_panic_test(void)
{
       kprintf("\n  Triggering test panic in 1 second...\n");
       timer_sleep(1000);
       panic("panic_test command - intentional panic");
}
static void dispatch(const char *line)
{
       line = trim_left(line);
       if (*line == '\0')
              return;
       const char *end = find_space(line);
       size_t nlen = (size_t)(end - line);
       for (size_t i = 0; i < CMD_COUNT; i++)
       {
              if (strlen(commands[i].name) == nlen &&
                  memcmp(commands[i].name, line, nlen) == 0)
              {
                     commands[i].handler(end);
                     return;
              }
       }
       vga_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
       kprintf("\n  unknown command: ");
       for (size_t i = 0; i < nlen; i++)
              vga_putchar(line[i]);
       kprintf("\n  type 'help' to list commands\n\n");
       vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
}

// looks good ?
static void print_banner(void)
{
       vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
       kprintf("\n");
       kprintf("  +-----------------------------------------+\n");
       kprintf("  |          helium  v0.1.0-alpha           |\n");
       kprintf("  |     type 'help' to list commands        |\n");
       kprintf("  +-----------------------------------------+\n");
       kprintf("\n");
       vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
}
void shell_run(void)
{
       char line[SHELL_LINE_MAX];

       print_banner();

       for (;;)
       {
              print_prompt();
              read_line(line, SHELL_LINE_MAX);
              dispatch(line);
       }
}
