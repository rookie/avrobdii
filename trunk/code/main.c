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

#include "system.h"
#include "delay.h"
#include "serial.h"
#include "time.h"
#include "leds.h"
#include "lcd.h"
#include "knob.h"
#include <avr/eeprom.h>
#include "flash.h" //external flash memeory routines
#include "iso.h"
#include "codes.h"
#include "obdii.h"



int main(void)
{
	TIME t;
	UINT8 f;
	UINT8 str[30];;
	UINT32 tp=0;
	INT16 knob=0;
	UINT8 reset;


	UINT8 msg[20];
	UBYTE resp_str[20];
	UINT8 ret;

	iso_init(10400);
	uart_init(115200); //Set up UART
	TimeInit();	 //set up time 
	LCD_init(); //initilize the LCD (DO BEFORE LEDS)

	LedsInit(); //set up LEDs
	LedsFlash(); //flash LEDs

	LED_POWER(1); //turn power LED on
	  
	//set up printf
	fdevopen(uart_putchar, NULL);

	//J1850Init();
	KnobInit(); //set up the knob processing

	

	flash_init();
	sprintf_P(str,"Stern Tech");
	LCD_print1(str,3);
	LCD_update();

	//turn on interrupts and let games begin
	ENABLE_INTERRUPTS();

	//printf_P("hello world %f\n",87.5);		

	/*	\
	msg[0]='A';
	msg[1]='A';

	while(1)
	{
		iso_put(msg,2,500);
	}

*/
	printf_P("OBDII running\n\r");
	LCD_clear();
	sprintf_P(str,"Checking ECM");	
	LCD_print1(str,0);
	sprintf_P(str,"Turn Key On");
	LCD_print2(str,0);
	
	KnobRead();
	reset=1;
	while(1)
	{
		INT temp; 
		temp=KnobRead();
		LCD_update();
		obdII_run(temp,reset);
		LCD_update();
		reset=0;
		//printf_P("battery %f\n\r",getBattery());
/*
		if (getBattery()<10.5)
		{
			if (getSeconds() & 0x0001)
			{
				LED_PWR(1);
			}else
			{
				LED_PWR(0);
			}
		}else*/
		{
			LED_POWER(1);
		}
		if (KnobSw1())
		{
			UBYTE knobtemp=0;
			UBYTE done=0;
			KnobRead();
			while(KnobSw1());
			while(!done)
			{
				//Do menu
				sprintf_P(str," Exit");
				if(knobtemp & 0x01)
				{
					 LCD_print2(str,0);
				} else
				{
					str[0]=0x7E;
					LCD_print1(str,0);
				}
				sprintf_P(str," Clear Code");
				if(knobtemp & 0x01)
				{
					 str[0]=0x7E;
					 LCD_print1(str,0);
				} else
				{
					LCD_print2(str,0);
				}
				if (KnobSw1())
				{
					while(KnobSw1());
					if(knobtemp & 0x01)
					{
						sprintf_P(str,"Clearing Codes");
						LCD_print1(str,0);
						sprintf_P(str," ");
						LCD_print2(str,0);
						temp=obdII_clear_codes();
						if (temp==0)
							temp=obdII_clear_codes();
						if (temp)
						{
							sprintf_P(str,"Codes cleared");
						}
						else
						{
							sprintf_P(str,"Error Clearing");
						}
						LCD_print1(str,0);
						if (temp)
						{
							sprintf_P(str," ");
						}
						else
						{
							sprintf_P(str,"Codes");
						}
						LCD_print2(str,0);
						obdII_leds_off();
						delay_ms(1000);
						delay_ms(1000);
						KnobSw1();
						knobtemp=0;
					} else
					{
						done=1;
						sprintf_P(str, "Wait");
						LCD_print1(str,0);
						sprintf_P(str, "Reading data");
						LCD_print2(str,0);
						reset=1;
					}
				}
				knobtemp+=KnobRead();
			}
		}

	}

	return 0;
	
}
