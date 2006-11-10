#ifndef __SYSTEM_H
#define __SYSTEM_H

#include "datatypes.h"

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

#include <avr/io.h>
#include <avr/interrupt.h>


#define SYSCLK 16000000UL//16Mhz
#define FCLK_IO SYSCLK 	//Fclk_I/O which is use for UART and timers

#define BIT_SET(x,n) (x=x | (0x01<<n))
#define BIT_TEST(x,n) ((x & (0x01<<n))!=0)
#define BIT_CLEAR(x,n) (x=x & ~(0x01<<n))

#define MAKE8(x,n) ((UINT8)((x>>(8*n)) & 0xFF))

#ifndef UBRR
#  define UBRR UBRRL
#endif

#define CLEAR_SCREEN() { \
		uart_putchar(27); \
		printf("[2J"); \
		uart_putchar(27); \
		printf("[f"); \
		}

#define DISABLE_INTERRUPTS() SREG=SREG & ~0x80
#define ENABLE_INTERRUPTS() SREG=SREG | 0x80

#define ClrWdt() 	//Restart watch dog timer
#define log_printf printf

//Error codes for Functions
#define NO_ERROR 0

#endif //__SYSTEM_H
