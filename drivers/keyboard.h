/* drivers/keyboard.h */
#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "../kernel/kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

/* PS/2 controller ports */
#define KB_DATA_PORT 0x60   /* read scan code / send command */
#define KB_STATUS_PORT 0x64 /* read status / send controller cmd */

/* Set 1 scan code: key released = scan code | 0x80 */
#define KB_RELEASED 0x80

void keyboard_init(void);
char keyboard_getchar(void); /* blocking — waits for next keypress */
int keyboard_haschar(void);  /* non-blocking poll */

#ifdef __cplusplus
}
#endif

#endif
