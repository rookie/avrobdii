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

/*******************************************************************
 *	File: led.c
 *
 * 	Copyright ©,  Trampas Stern. All Rights Reserved.	  
 *  Date: 5/9/2006	8:56:29 PM
 *******************************************************************/
#include "leds.h"
#include "scanner.h" //for data bus R/Ws	
#include "delay.h"		


//Mirror the 74AHC573 register contents
UINT8 LEDS_U18;
UINT8 LEDS_U40;

/******************************************************************
 ** LedsInit
 *
 *  DESCRIPTION:
 *	 Sets up the LEDS
 *	
 *  Create: 5/9/2006	9:28:19 PM - Trampas Stern
 *******************************************************************/
INT LedsInit(void)
{
	//first we need to set up ports
	DDRG=DDRG | 0x04; //PG2 as output latch U18
	DDRC=DDRC | 0x80; //PC7 as output latch U40
	
	LEDS_U18=0xFF;
	LEDS_U40=0xFF;
	//drive the latches low
	BIT_CLEAR(PORTG,2);
	BIT_CLEAR(PORTC,7);

	//Turn all the LEDS off 
	DATA_WRITE(0xFF);

	//latch data
	BIT_SET(PORTG,2);
	BIT_SET(PORTC,7);

	//drive the latches low
	BIT_CLEAR(PORTG,2);
	BIT_CLEAR(PORTC,7);

	return NO_ERROR;
}

/******************************************************************
 ** LedsUpdate
 *
 *  DESCRIPTION:
 *		Updates the LEDs with data from global state register
 *
 *  Create: 5/9/2006	9:28:49 PM - Trampas Stern
 *******************************************************************/
INT LedsUpdate(void)
{
	//write data to U18
	DATA_WRITE(LEDS_U18);
	//latch data
	BIT_SET(PORTG,2);
	BIT_SET(PORTG,2); //write twice as delay
	BIT_CLEAR(PORTG,2);

	//write data to U40
	DATA_WRITE(LEDS_U40);
	//latch data
	BIT_SET(PORTC,7);
	BIT_SET(PORTC,7); //write twice as delay
	BIT_CLEAR(PORTC,7);

	return NO_ERROR;
}

/******************************************************************
 ** LedsSet
 *
 *  DESCRIPTION:
 *		Sets the state of the leds
 *
 *  Create: 5/9/2006	9:37:35 PM - Trampas Stern
 *******************************************************************/
INT LedsSet(UINT8 Led, UINT8 State)
{
	if (Led<=7)
	{
		if (State)
	   	{
	   		BIT_CLEAR(LEDS_U18,Led);
		}else
		{
			BIT_SET(LEDS_U18,Led);
		}
	}else if (Led<=14)
	{
		Led=Led-8;
		if (State)
	   	{
	   		BIT_CLEAR(LEDS_U40,Led);
		}else
		{
			BIT_SET(LEDS_U40,Led);
		}
	}
	return LedsUpdate(); //update the LEDs
}

/******************************************************************
 ** LedsFlash
 *
 *  DESCRIPTION:
 *		Flashes all the Leds
 *
 *  Create: 5/9/2006	9:41:43 PM - Trampas Stern
 *******************************************************************/
INT LedsFlash(void)
{
	UINT8 i;

	for (i=1; i<15; i++)
	{
		LEDS_U18=0x00;
		LEDS_U40=0x00;
		LedsUpdate();
		delay_ms(100);
		LEDS_U18=0xFF;
		LEDS_U40=0xFF;
		LedsUpdate();
		delay_ms(100);
	}
	return NO_ERROR;
}



		


		



	

