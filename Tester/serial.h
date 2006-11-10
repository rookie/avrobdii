#ifndef _SERIAL_H
#define _SERIAL_H
#include "system.h"

void uart_init(UDWORD baud);
int uart_putchar(char c);
UBYTE uart_kbhit(void);
UBYTE uart_getchar(void);




#endif //_SERIAL_H
