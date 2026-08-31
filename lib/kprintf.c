
/* lib/kprintf.c
 * Minimal printf: supports %c %s %d %u %x %p %%.
 * No floating point — freestanding kernels don't need it. */
#include "kprintf.h"
#include "string.h"
#include "../drivers/vga.h"

/* va_list without stdarg.h */
typedef __builtin_va_list va_list;
#define va_start(v, l) __builtin_va_start(v, l)
#define va_arg(v, t) __builtin_va_arg(v, t)
#define va_end(v) __builtin_va_end(v)

static void print_uint(uint32_t n, uint32_t base)
{
       static const char digits[] = "0123456789abcdef";
       char buf[16];
       int i = 0;
       if (!n)
       {
              vga_putchar('0');
              return;
       }
       while (n)
       {
              buf[i++] = digits[n % base];
              n /= base;
       }
       while (i--)
              vga_putchar(buf[i]);
}

static void print_int(int32_t n)
{
       uint32_t u = (uint32_t)n;
       if (n < 0)
       {
              vga_putchar('-');
              u = (uint32_t)(-(int64_t)n);
       }
       print_uint(u, 10);
}

void kprintf(const char *fmt, ...)
{
       va_list ap;
       va_start(ap, fmt);
       for (; *fmt; fmt++)
       {
              if (*fmt != '%')
              {
                     vga_putchar(*fmt);
                     continue;
              }
              switch (*++fmt)
              {
              case 'c':
                     vga_putchar((char)va_arg(ap, int));
                     break;
              case 's':
              {
                     const char *s = va_arg(ap, const char *);
                     if (!s)
                            s = "(null)";
                     while (*s)
                            vga_putchar(*s++);
                     break;
              }
              case 'd':
                     print_int(va_arg(ap, int32_t));
                     break;
              case 'u':
                     print_uint(va_arg(ap, uint32_t), 10);
                     break;
              case 'x':
                     print_uint(va_arg(ap, uint32_t), 16);
                     break;
              case 'p':
                     vga_puts("0x");
                     print_uint(va_arg(ap, uint32_t), 16);
                     break;
              case '%':
                     vga_putchar('%');
                     break;
              default:
                     vga_putchar('%');
                     vga_putchar(*fmt);
                     break;
              }
       }
       va_end(ap);
}
