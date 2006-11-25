/*
Copyright (C) Trampas Stern  name of author

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

/*******************************************************************
 *	File: knob.c
 *
 * 	Copyright ©,  Trampas Stern. All Rights Reserved.	  
 *  Date: 5/12/2006	9:06:29 PM
 *******************************************************************/
#include "knob.h"

#define KNOB_READ ((PINE & 0x30)>>4) //reads the encoder pins
#define KNOB_ENABLE_ISR() {EIMSK=EIMSK | 0x30;} 
#define KNOB_DISABLE_ISR() {EIMSK=EIMSK & 0xCF;} 

volatile UINT8 KnobState=0x00;    //state of knob and switches

volatile INT8 Knob; //counter as to number of turns on knob

volatile UINT8 KnobSwitch=0;

/******************************************************************
 ** KnobInit
 *
 *  DESCRIPTION:
 *	 Sets up the ISR for the knob
 *
 *  Create: 5/12/2006	9:08:07 PM - Trampas Stern
 *******************************************************************/
INT KnobInit(void)
{
	//first we need to disable the interrupts
	//not that encoder is on INT4 & INT5 
	KNOB_DISABLE_ISR(); //lower the INTERUPT masks for knob

	//next configure the interrupt as change interupts
	EICRB=(EICRB & 0xC0) | 0x15;

	//configure the knob pins as inputs
	DDRE=DDRE & 0x8F;

	//reset state variables
	Knob=0;
	KnobSwitch=0;
		
	//enable the interrupts
	KNOB_ENABLE_ISR();

	return NO_ERROR;
}

/******************************************************************
 ** INT4 ISR
 *
 *  DESCRIPTION:  ISR for INT4
 *
 *  Create: 5/12/2006	9:18:21 PM - Trampas Stern
 *******************************************************************/
ISR(INT4_vect)
{
	if (BIT_TEST(PINE,4)== BIT_TEST(PINE,5))
		Knob++;	   //this should be atomic
	else
		Knob--;		//this should be atomic	

/*UINT8 state;
	UINT8 temp;

	//read current state
	state=KNOB_READ;

	//compare against previous state;
	temp=state ^ KnobState;

	temp=temp & 0x03;
	//OK lets see if we have a knob turning... 
	if (temp)
	{
		if (BIT_TEST(KnobState,0)==BIT_TEST(KnobState,1))
		{
			temp=temp ^ 0x03; 
		}

		if (temp==0x01)
			Knob++;	   //this should be atomic
		else
			Knob--;		//this should be atomic
	}

	KnobState=state;
*/
}

/******************************************************************
 ** INT5 ISR
 *
 *  DESCRIPTION:  ISR for INT5
 *
 *  Create: 5/12/2006	9:18:21 PM - Trampas Stern
 *******************************************************************/

ISR(INT5_vect)
{
	
	if (BIT_TEST(PINE,4)== BIT_TEST(PINE,5))
		Knob--;	   //this should be atomic
	else
		Knob++ ;		//this should be atomic	
}

	
	/*UINT8 state;
	UINT8 temp;

	//read current state
	state=KNOB_READ;

	//compare against previous state;
	temp=state ^ KnobState;

	temp=temp & 0x03;
	//OK lets see if we have a knob turning... 
	if (temp)
	{
		if (BIT_TEST(KnobState,0)==BIT_TEST(KnobState,1))
		{
			temp=temp ^ 0x03; 
		}

		if (temp==0x01)
			Knob++;	  //this should be atomic
		else
			Knob--;	 //this should be atomic

	}

	KnobState=state;
}
*/

/******************************************************************
 ** INT6 ISR
 *
 *  DESCRIPTION:  ISR for INT6
 *
 *  Create: 5/12/2006	9:18:21 PM - Trampas Stern
 *******************************************************************/
/*ISR(INT6_vect)
{
   KnobSwitch = ((PORTE & 0x40)==0);
}*/



/******************************************************************
 ** KnobRead
 *
 *  DESCRIPTION:
 *
 *  Create: 5/12/2006	9:27:03 PM - Trampas Stern
 *******************************************************************/
INT KnobRead(void)
{
	INT temp;

	temp=Knob;
	Knob=0;		   //this should be atomic
	return temp;
}


/******************************************************************
 ** KnobSw1
 *
 *  DESCRIPTION:
 *
 *  Create: 5/12/2006	9:29:42 PM - Trampas Stern
 *******************************************************************/
INT KnobSw1(void)
{
	return ((PINE & 0x40)==0);
}
