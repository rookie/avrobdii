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

#include "LCD.h"
#include "delay.h"

UBYTE str1[80]={0};
UBYTE line1=0;
UBYTE str2[80]={0};
UBYTE line2=0;


/******************************************************************
 ** LCD_check_busy
 *
 *  DESCRIPTION:
 *
 *  Create: 5/9/2006	10:04:36 PM - Trampas Stern
 *******************************************************************/
void LCD_check_busy()
{
	UBYTE i;
	i=0;
	LCD_RS(0);
	LCD_RW(1);
    LCD_ENABLE(1);
	DDRA=0x00;
	while ((LCD_DATA & 0x80) && i<250)
	{
		delay_us(50);
		i++;
	}
	LCD_ENABLE(1);
}

/******************************************************************
 ** LCD_Send_command
 *
 *  DESCRIPTION:
 *
 *  Create: 5/9/2006	10:04:47 PM - Trampas Stern
 *******************************************************************/
void LCD_Send_command(UBYTE command)
{
	LCD_check_busy();
	LCD_RS(0);
	LCD_RW(0);
	LCD_ENABLE(1);
	LCD_DATA_WRITE(command);
	delay_us(100);	 
	LCD_ENABLE(0);
	delay_us(100);	
	LCD_RW(1);
}


/******************************************************************
 ** LCD_Send_data
 *
 *  DESCRIPTION:
 *
 *  Create: 5/9/2006	10:05:03 PM - Trampas Stern
 *******************************************************************/
void LCD_Send_data(UBYTE data)
{
	LCD_check_busy();
	LCD_RS(1);
	LCD_RW(0);
	LCD_ENABLE(1);
	LCD_DATA_WRITE(data);
	delay_us(100);	//200
	LCD_ENABLE(0);
	delay_us(100); //200
	LCD_RW(1);
}

void LCD_init()
{  

#if  USE_LCD

	
	//first step in setting up LCD is to 
	// turn ports to ouput as needed
	LCD_ENABLE(1);
	BIT_SET(DDRC,0); //set RS as output
	BIT_SET(DDRG,0); //set enable as output
	BIT_SET(DDRG,1); //set R/W as output


	

	LCD_Send_command(0x30);
	delay_ms(6);
    LCD_Send_command(0x30);
    LCD_Send_command(0x30);
	LCD_Send_command(0x38);
	LCD_Send_command(0x0C); //no cursor
	LCD_Send_command(0x06);
	LCD_Send_command(0x02);
	LCD_Send_command(0x01);
	LCD_ENABLE(0);
	LCD_RW(1);
	
#endif
}	

void LCD_clear()
{
#if  USE_LCD
	str2[0]=0;
	LCD_Send_command(0x01);
#endif
}

void LCD_blackout()
{
#if  USE_LCD

	UBYTE i;
	for (i=0;i<LCD_WIDTH; i++)
	{
		LCD_Send_command(0x80+i);
		LCD_Send_data(0xFF);
		LCD_Send_command(0xC0+i);
		LCD_Send_data(0xFF);
	}
#endif
}
		
	
//print on line 1
void LCD_print1(UBYTE *data, UBYTE pos)
{
#if  USE_LCD
	UBYTE addr;
	UBYTE i;

	if (pos>LCD_WIDTH)
		return;

	sprintf_P(str1,"%s",data);
	line1=strlen(str1);;
	addr=0x80+pos;
	i=0;
	LCD_Send_command(addr);
	while (*data !=0 && i<(LCD_WIDTH-pos))
	{
		LCD_Send_data(*data);
		data++;
		i++;
	}
	while(i<(LCD_WIDTH-pos))
	{
	  	LCD_Send_data(' ');
		i++;
	}

#endif
}
	
//print on line 2
void LCD_print2(UBYTE *data, UBYTE pos)
{
#if  USE_LCD

	UBYTE addr;
	UBYTE i;

	if (pos>LCD_WIDTH)
		return;
	addr=0xC0+pos;
	LCD_Send_command(addr);
	i=0;
	sprintf_P(str2,"%s",data);
	line2=strlen(str2);;
	while (data[i] !=0 && i<(LCD_WIDTH-pos))// && addr<=(0xC0+LCD_WIDTH))
	{
		LCD_Send_data(data[i]);
		i++;
	}
	while(i<(LCD_WIDTH-pos))
	{
	  	LCD_Send_data(' ');
		i++;
	}
	
	
	
#endif
}

void LCD_update()
{
	UBYTE len,i;
	static UBYTE temp=0;

	//printf_P("LCD timer %lu\n\r",lcd_timer);
//	if(lcd_timer<2)
//		return;
	
	//disable_interrupts(GLOBAL);
//	lcd_timer=0;
	//enable_interrupts(GLOBAL);

	len=strlen(str2);
	if (len>LCD_WIDTH)
	{
		if ((line2+LCD_WIDTH)<=len)
		{
			LCD_Send_command(0xC0);	 //set LCD address line 2
		   
			for (i=line2; i<len && i<(line2+LCD_WIDTH); i++)
			{
				LCD_Send_data(str2[i]);
			}
		}

		if(line2==0)
		{
			temp++;
			if (temp>=(LCD_WIDTH/2))
			{
				line2++;
				temp=0;
			}else
			{
				return;
			}
		} else
		{
			line2++;
		}
		if (line2>=(len-LCD_WIDTH/2))
		{
			line2=0;
		}
	}	
	
}
