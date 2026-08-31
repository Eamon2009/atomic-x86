/* drivers/vga.c
 * VGA text-mode driver. Writes directly to the memory-mapped
 * framebuffer at 0xB8000 (80x25 cells, 2 bytes each). */

#include "vga.h"
static uint8_t vga_col = 0;
static uint8_t vga_row = 0;
static uint8_t vga_attr = 0; /* current colour attribute byte */
static inline uint8_t make_color(vga_color_t fg, vga_color_t bg)
{ return (uint8_t)((bg << 4) | fg); }
static inline void put_entry(char c, uint8_t attr, uint8_t col, uint8_t row)
{
    uint16_t entry = (uint16_t)c | ((uint16_t)attr << 8);
    VGA_BUFFER[row * VGA_COLS + col] = entry;
}
static void update_cursor(void)
{
    uint16_t pos = (uint16_t)(vga_row * VGA_COLS + vga_col);
    outb(0x3D4, 0x0F); /* cursor low byte index */
    outb(0x3D5, (uint8_t)(pos));
    outb(0x3D4, 0x0E); /* cursor high byte index */
    outb(0x3D5, (uint8_t)(pos >> 8));
}
static void scroll(void)
{
    /* move every row up by one */
    for (uint8_t r = 1; r < VGA_ROWS; r++)
        for (uint8_t c = 0; c < VGA_COLS; c++)
            VGA_BUFFER[(r - 1) * VGA_COLS + c] = VGA_BUFFER[r * VGA_COLS + c];

    /* blank the last row */
    uint16_t blank = (uint16_t)' ' | ((uint16_t)vga_attr << 8);
    for (uint8_t c = 0; c < VGA_COLS; c++) VGA_BUFFER[(VGA_ROWS - 1) * VGA_COLS + c] = blank;

    vga_row = VGA_ROWS - 1;
}
void vga_init(void)
{
    vga_attr = make_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    vga_col = 0;
    vga_row = 0;
    vga_clear();
}

void vga_clear(void)
{
    uint16_t blank = (uint16_t)' ' | ((uint16_t)vga_attr << 8);
    for (uint16_t i = 0; i < VGA_COLS * VGA_ROWS; i++) VGA_BUFFER[i] = blank;
    vga_col = 0;
    vga_row = 0;
    update_cursor();
}

void vga_set_color(vga_color_t fg, vga_color_t bg)
{ vga_attr = make_color(fg, bg); }

void vga_putchar(char c)
{
    if (c == '\n') {
        vga_col = 0;
        if (++vga_row == VGA_ROWS) scroll();
    } else if (c == '\r') {
        vga_col = 0;
    } else if (c == '\b') {
        if (vga_col > 0) {
            vga_col--;
            put_entry(' ', vga_attr, vga_col, vga_row);
        } else if (vga_row > 0) {
            vga_row--;
            vga_col = VGA_COLS - 1;
            put_entry(' ', vga_attr, vga_col, vga_row);
        }
    } else if (c == '\t') {
        vga_col = (uint8_t)((vga_col + 8) & ~7u); /* next 8-stop */
        if (vga_col >= VGA_COLS) {
            vga_col = 0;
            if (++vga_row == VGA_ROWS) scroll();
        }
    } else {
        put_entry(c, vga_attr, vga_col, vga_row);
        if (++vga_col == VGA_COLS) {
            vga_col = 0;
            if (++vga_row == VGA_ROWS) scroll();
        }
    }
    update_cursor();
}

void vga_puts(const char *s)
{
    while (*s) vga_putchar(*s++);
}
