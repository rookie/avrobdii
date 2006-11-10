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

#ifndef __LCD_H
#define __LCD_H

#include "scanner.h"

//Set up the pins for the LCD
#define LCD_RS(x) {if(x) BIT_SET(PORTC,0); else  BIT_CLEAR(PORTC,0);}
#define LCD_RW(x) {if(x) BIT_SET(PORTG,0); else  BIT_CLEAR(PORTG,0);}
#define LCD_ENABLE(x) {if(x) BIT_SET(PORTG,1); else  BIT_CLEAR(PORTG,1);}
#define LCD_DATA PINA
#define LCD_DATA_WRITE(x) DATA_WRITE(x)

#define LCD_WIDTH 16 //Number of chars width of LCD



void LCD_init();
void LCD_clear();
void LCD_blackout();

void LCD_print1(UBYTE *data, UBYTE pos);
void LCD_print2(UBYTE *data, UBYTE pos);

void LCD_Send_command(UBYTE command);
void LCD_Send_data(UBYTE data);
void LCD_update();

#endif //__LCD_H


