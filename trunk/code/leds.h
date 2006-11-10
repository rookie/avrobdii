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
 *	File: leds.h
 *
 * 	Copyright ©,  Trampas Stern. All Rights Reserved.	  
 *  Date: 5/9/2006	8:56:19 PM
 *******************************************************************/
#ifndef __LEDS_H
#define __LEDS_H

#include "system.h"

INT LedsInit(void);
INT LedsSet(UINT8 Led, UINT8 State); //turns LEDs on or off
INT LedsFlash(void); //Flashes the LEDs

//This module controls the LEDs using the 74AHC573s

#define LED_DTC(x) LedsSet(13,x)
#define LED_CAT(x) LedsSet(0,x)
#define LED_HEATED_CAT(x) LedsSet(1,x)
#define LED_EVAP(x) LedsSet(2,x)
#define LED_AIR(x) LedsSet(3,x)
#define LED_O2(x) LedsSet(12,x)
#define LED_HEATED_O2(x) LedsSet(11,x)
#define LED_EGR(x) LedsSet(10,x)
#define LED_AC(x) LedsSet(4,x)
#define LED_MISFIRE(x) LedsSet(9,x)
#define LED_COMP(x) LedsSet(5,x)
#define LED_FUEL(x) LedsSet(8,x)
#define LED_POWER(x) LedsSet(14,x)

//NOTE 7 & 6 are unused Leds 


#endif //__LEDS_H

