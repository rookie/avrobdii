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

#include "codes.h"
#include "lcd.h"
#include "flash.h"	 


/*
 ** CodeGet
 *
 *  FILENAME: D:\OBDII\Code\codes.c
 *
 *  PARAMETERS:
 *
 *  DESCRIPTION: Gets code information from flash
 *
 *  RETURNS: ptr in flash to code
			0- error
 *
 */
#define MAX_CODE_STR 80
#define TYPE_P 'P'
#define TYPE_C 'C'
#define TYPE_B 'B'
#define TYPE_U 'U'
UDWORD PcodePrint(UDWORD code)
{
	UDWORD data,addr=0,offset,next; 
	UBYTE done=0,i,temp;
	UBYTE str[MAX_CODE_STR];
	UBYTE type=0;

	if (code>=0 && code<10000)
	{
		type=TYPE_P;
		addr=PCODE_FLASH_START;
	}
	if (code>=10000 && code<20000)
	{
		type=TYPE_C;
		addr=CCODE_FLASH_START;
		code=code-10000;
	}
	if (code>=20000 && code<30000)
	{
		type=TYPE_B;
		addr=BCODE_FLASH_START;
		code=code-20000;
	}
	if (code>=30000)
	{
		type=TYPE_U;
		addr=UCODE_FLASH_START;
		code=code-30000;
	}

	
	while(!done)
	{
		data=flash_read(addr++);
		data=(data<<8) | flash_read(addr++);
		data=(data<<8) | flash_read(addr++);
		data=(data<<8) | flash_read(addr++);

		//printf_P("Current code %lu, %lX\n\r",data,data);
		offset=flash_read(addr++);
		offset=(offset<<8) | flash_read(addr++);
		offset=(offset<<8) | flash_read(addr++);
		offset=(offset<<8) | flash_read(addr++);
		next=addr+offset;
		//printf_P("next is %lX = %lX(offset) + %lX(addr)\n\r",next,offset,addr);
		if (data==code)
		{
			//display code on LCD
			LCD_clear();
			sprintf_P(str,"%c%04lu",type,code);
			//printf("%s",str);
			LCD_print1(str,2);
			i=0;
			temp=flash_read(addr++);
			while (i<(MAX_CODE_STR-1) && temp!=0)
			{
				str[i++]=temp;
				temp=flash_read(addr++);
			}
			str[i++]=0;
			//printf_P("\t%s\n\r",str);

			LCD_print2(str,0);
			done=1;
		}
		if (data==0xFFFFFFFF || data==0)
		{
			LCD_clear();
			sprintf_P(str,"%c%04lu",type,code);
			//printf_P("%s",str);
			LCD_print1(str,2);
			sprintf_P(str,"Undocumented %04lu",code);
			//printf_P("\t%s\n\r",str);
			LCD_print2(str,0);
			done=1;
		}

		
		addr=next;
	}

	

	return 0;
}

