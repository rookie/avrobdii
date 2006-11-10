#ifndef _SERIAL2_H
#define _SERIAL2_H
#include "system.h"

void uart1_init(UDWORD baud);
int uart1_putchar(char c);
UBYTE uart1_kbhit(void);
UBYTE uart1_getchar(void);




#endif //_SERIAL2_H
