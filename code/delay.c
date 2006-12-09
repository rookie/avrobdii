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

#include "delay.h"



/*********************************************************
Date: 11/17/04
Author: Trampas Stern

This function delays a number of micro seconds
********************************************************/
UBYTE delay_us(UBYTE x)
{
	
	while(x--)
	{
		asm volatile (" NOP");
		asm volatile (" NOP");
		asm volatile (" NOP");
		asm volatile (" NOP");
		asm volatile (" NOP");
		asm volatile (" NOP");
		asm volatile (" NOP");
		asm volatile (" NOP");
		asm volatile (" NOP");
		asm volatile (" NOP");
		asm volatile (" NOP");
		asm volatile (" NOP");
		asm volatile (" NOP");
	}
	return 0;
}

/*********************************************************
Date: 11/17/04
Author: Trampas Stern

This function delays a number of milli seconds
********************************************************/
UBYTE delay_ms(UINT16 time)
{
	UBYTE i;
	while(time--)
	{
		for(i=0; i<4; i++)
		{
			delay_us(250);
		}
	}
	return 0;
}

