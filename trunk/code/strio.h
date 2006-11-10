/*
Copyright (C) Trampas Stern  name of author

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef __STRIO_H
#define __STRIO_H
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "serial.h"
//#include "datatypes.h"

#define log_printf printf

#define MAX_STRING 50 //number of chars

#define MAX_MANTISA 6	//max number of chars for float mantissa

#define putch uart_putchar


#define CLEAR_SCREEN() { \
		putch(27); \
		printf("[2J"); \
		putch(27); \
		printf("[f"); \
		}

#define NEW_LINE() {puts("\n\r");}


int tbs_ltoa(CHAR *str, long lval, int sign);
int ftoa (float x, CHAR *str, char  prec, char format);
int sprintf(CHAR *str,  void *fmt, ...);
int printf(void *fmt, ...);
int sprintf_va(CHAR *str,CHAR *fmt , va_list ap);

int puts(CHAR *str);
int putsf(float x, char precision);
int putsx(long data);
int putsd(long data);
int tbsstrcpy(const CHAR *src, CHAR *dst);
//int strlen(const CHAR *str);
//int strcmp(c
int hextoa(CHAR *str, long data, char lowerCase);

//FLOAT atof(CHAR *str);

#endif //__STRIO_H
