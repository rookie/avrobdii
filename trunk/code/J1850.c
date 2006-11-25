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
 *	File: J1850.c
 *
 * 	Copyright ©,  Trampas Stern. All Rights Reserved.	  
 *  Date: 5/19/2006	9:34:55 PM
 *******************************************************************/
#include "scanner.h"
#include "time.h"

#define PWM_UNKNOWN 0
#define PWM_BUS_ERROR 1
#define PWM_SOF  2
#define PWM_DATA 3
#define PWM_EOD  4
#define PWM_EOF  5
#define PWM_IDLE 6

#define PWM_BIT_US 24 //us for bit time
#define PWM_EOD_US 51 //us for End of Data time
#define PWM_EOF_US 72 //us for End of Frame time 
#define PWM_IDLE_US 96 //us till idle 


#define TP1 8
#define TP2 16
#define TP3 24
#define TP4 48
#define TP5 72
#define TP7 32

#define PWM_OUT(x) {if (x) BIT_SET(PORTB,5); else BIT_CLEAR(PORTB,5);}
#define PWM_IN BIT_TEST(ACSR,5)


#define J1850_ARRAY_N 16
#define J1850_ARRAY_PKTS 4	 //must be power of 2
UINT8 J1850Data[J1850_ARRAY_PKTS][J1850_ARRAY_N];
UINT8 J1850DataBytes[J1850_ARRAY_PKTS];
UINT8 J1850DataTime[J1850_ARRAY_PKTS];

volatile UINT8 J1850_dataRead;
volatile UINT8 J1850_dataWrite;

volatile UINT8 PWMState;

volatile UINT8 *ptrWrite;

volatile UINT8 NumPktBytes;

/****************
 Basic Operation
	The J1850 comes in two flavors a PWM (pluse width modulation) and VPW 
	(variable pluse width modulation). For our initial system what we 
	are going to do is have the analog comparitor trigger on rising and 
	falling edges. Then we will have this trigger the timer/caputre (TC1). 
	This will tell allow us to measure the pulse widths. 
*****************/


/******************************************************************
 ** J1850Init
 *
 *  DESCRIPTION:
 *
 *  Create: 5/19/2006	9:35:25 PM - Trampas Stern
 *******************************************************************/
INT J1850Init(void)
{
	//Set up data direction registers
	BIT_SET(DDRB,6); //set OUT_VPW as output
	BIT_SET(DDRB,5); //set OUT_PWM as output
												 
	//ENABLE PWM
	//BIT_SET(DDRC,2); //set pin as ouput
	//BIT_CLEAR(PORTC,2); //drive pin low

	BIT_CLEAR(PORTB,6);//Turn off OUT_VPW
	BIT_CLEAR(PORTB,5);//Turn off OUT_PWM



	//setup the analog comparator 
	ADCSRB=0x00; //use negative input on compartor
	ACSR=0x08; 

	//Output capture mode all disconnected
	TCCR1A=0x00;

	//noise canceler on (bit 7)
	// prescaller clk/8==tick every half microSecond
 	TCCR1B=0x82;

	//ignore the force output compare 
	TCCR1C=0x00;

	//set count to zero
	TCNT1=0x00;

	//The timer is reset on the rising edge of bits
	//the EOD,EOF,and Idle time are based on end of last bit
	// Thus we have to add bit time to each
	//double due to timer runing twice as fast 
	OCR1A=(PWM_EOD_US+PWM_BIT_US)*2; //EOD time
	OCR1B=(PWM_EOF_US+PWM_BIT_US)*2;	//EOF time
	OCR1C=(PWM_IDLE_US+PWM_BIT_US)*2;	//IDLE time

	TIMSK1=0x0E;

	J1850_dataWrite=0;
	J1850_dataRead=0;
	NumPktBytes=0;
	ptrWrite=J1850Data[J1850_dataWrite];
	PWMState=PWM_UNKNOWN;
	return NO_ERROR;
}



ISR(TIMER1_COMPA_vect)
{
	if (BIT_TEST(ACSR,5)==0) //passive bus state
	{
		if (PWMState<PWM_EOD)
		{
			PWMState=PWM_EOD;
			if (NumPktBytes>0)
			{
				J1850DataBytes[J1850_dataWrite]=NumPktBytes;
				J1850_dataWrite++;
				J1850_dataWrite=J1850_dataWrite &  (J1850_ARRAY_PKTS-1);
				ptrWrite=J1850Data[J1850_dataWrite];
							}
			NumPktBytes=0;			
		}
	}else
	{
		PWMState=PWM_BUS_ERROR;
	}		
}

ISR(TIMER1_COMPB_vect)
{
	if (BIT_TEST(ACSR,5)==0) //passive bus state
	{
		PWMState=PWM_EOF;
	}else
	{
		PWMState=PWM_BUS_ERROR;
	}	
}

ISR(TIMER1_COMPC_vect)
{
	if (BIT_TEST(ACSR,5)==0) //passive bus state
	{
		PWMState=PWM_IDLE;
	}else
	{
		PWMState=PWM_BUS_ERROR;
	}
	// Note we do not want the timer to overflow so 
	//  we reset it to some nominal amount. 
	//TCNT1=48*2+1;		
}

/******************************************************************
 ** Analog Comparator ISR
 *
 *  DESCRIPTION:
 *
 *  Create: 6/4/2006	9:21:43 AM - Trampas Stern
 *******************************************************************/
ISR(ANALOG_COMP_vect)
{
	static UINT8 data=0;
	static UINT8 bits=0;
	
	UINT8 t;

	//read timer1
	t=TCNT1L;
	if (BIT_TEST(ACSR,5)!=0)
	{
		//rising edge
		TCNT1L=0x00;
		if (PWMState>PWM_DATA)
		{
			J1850DataTime[J1850_dataWrite]=t;
		}
				
	}else  //we have falling edge
  	{
		//Falling edge
		if (t<=39 && t>=14)
		{
			PWMState=PWM_DATA;
			data=data<<1; 
			if (t<=24)
				data=data | 0x01;
			bits++;
			if (bits>7)
			{
				*ptrWrite=data;
				NumPktBytes=NumPktBytes+1;
				if (NumPktBytes<J1850_ARRAY_N)
				{
					ptrWrite++;
					//if (ptrWrite>(J1850Data	+ (4*16)))
					//{
					//	ptrWrite--;
					//}
				}				
				bits=0;
			}
		}
		if (t>=60 && t<=66)
		{
			PWMState=PWM_SOF;
			bits=0;
		}
	}

}


/******************************************************************
 ** J1850test
 *
 *  DESCRIPTION:
 *
 *  Create: 6/4/2006	9:33:15 AM - Trampas Stern
 *******************************************************************/
INT J1850test(void)
{
	UBYTE i;	


	if (J1850_dataWrite!=J1850_dataRead)
	{
	    printf_P("\nPWMSTATE=%d\n",PWMState);
	    printf_P("J1850_DATA %d(us) ",J1850DataTime[J1850_dataRead]/2);
		for (i=0; i<J1850DataBytes[J1850_dataRead]; i++)
		{
			UINT8 d;
			d=J1850Data[J1850_dataRead][i];
			printf_P("%X ",d);	
		}
		J1850_dataRead++;
		J1850_dataRead=J1850_dataRead &	(J1850_ARRAY_PKTS-1);

	}

	return NO_ERROR; 
}

/******************************************************************
 ** PWM_get
 *
 *  DESCRIPTION: Gets a PWM packet
 *
 *  Create: 6/11/2006	7:05:01 PM - Trampas Stern
 *******************************************************************/
INT pwm_get(UINT8 *ptr, UINT8 count, UINT16 time_out_ms)
{
	TIME ptrTime;
	UINT8 i=0;
	//get current time
	GetTime(&ptrTime);
/*
 	while (!done)
	{
	 	if (J1850_dataWrite!=J1850_dataRead)
		{
		    //printf_P("\nPWMSTATE=%d\n",PWMState);
		    //printf_P("J1850_DATA %d(us) ",J1850DataTime[J1850_dataRead]/2);
			for (i=0; i<J1850DataBytes[J1850_dataRead] && i<count; i++)
			{
				//UINT8 d;
				*ptr++=J1850Data[J1850_dataRead][i];
				//printf_P("%X ",d);	
			}
			J1850_dataRead++;
			J1850_dataRead=J1850_dataRead &	(J1850_ARRAY_PKTS-1);

		}

		//check timne
		if (GetElaspMs(ptrTime)>time_out_ms)
		{
			done=1;
		}
	} //while
*/
	return i;
}

/******************************************************************
 ** pwm_put
 *
 *  DESCRIPTION:
 *
 *  Create: 6/11/2006	7:10:50 PM - Trampas Stern
 *******************************************************************/
INT pwm_put(UINT8 *data, UINT8 count, UINT16 time_out_ms)
{
	
	TIME ptrTime;
	UINT8 done=0;
	UINT8 i=0;
	INT8 bit;
	UINT8 time;
	//get current time
	GetTime(&ptrTime);


	while (!done)
	{
		//first check that the PWM bus is idle 
		if (PWMState>=PWM_IDLE)
		{
			//disable the receive ISRs
			TIMSK1=0x00;
			ACSR=0x00; 

			//Transmit SOF
			TCNT1L=0x00;
			PWM_OUT(1);
			while (TCNT1L<TP7);
			if (PWM_IN!=1)
			{
				//enable reciever
				ACSR=0x08; 
				TIMSK1=0x0E;
				return 0;
			}
			while(TCNT1L<(TP7*2));
			//drive low
			PWM_OUT(0);
			while(TCNT1L<(TP4*2));
			TCNT1L=0x00;
			PWM_OUT(1);

		
			//now lets drive the bits
			for (i=0; i<count; i++)
			{
				UINT8 t;
				t=*data++;
				for (bit=7; bit>=0; bit--)
				{
					UINT8 b;
					b=(t>>bit) & 0x01;
					time=TP1;
					if (b==0)
					{
						time=TP2;
					}
					while(TCNT1L<(time));
					if (PWM_IN!=1)
					{
						//drive low
						PWM_OUT(0);
						//enable reciever
						ACSR=0x08; 
						TIMSK1=0x0E;
						return 0;
					}
					while(TCNT1L<(time*2));
					PWM_OUT(0);
				   	while(TCNT1L<(TP3*2));
				   	TCNT1L=0x00;
					PWM_OUT(1);	
				}
			}
		} //if PWMstate
		//check time
		if (GetElaspMs(&ptrTime)>time_out_ms)
		{
			done=1;
		}
	} //while(!done) 
	//drive low
	PWM_OUT(0);			
	//enable reciever
	ACSR=0x08; 
	TIMSK1=0x0E;
	return i;
}




//does CRC calculations
UINT8 crc(UINT8 *data, UINT8 len)
{
	UINT8 result;
	UINT8 i;
	UINT8 mask;
	UINT8 j;
	UINT8 temp;
	UINT8 poly;

	result=0xFF;
	for(i=0; i<len; i++)
	{
		mask=0x80;
		temp=data[i];
		for(j=0; j<8; j++)
		{
			if(temp & mask)	  //bit is 1
			{
				poly=0x1c;	
				if(result & 0x80)
				{
					poly=1;
				}
				result= ((result<<1) | 0x01) ^ poly;

			}else
			{
			 	poly=0;
				if(result & 0x80)
				{
					poly=0x1D;
				}  
				result= ((result<<1)) ^ poly;

			}
			mask=mask>>1;
		}
	}
	return ~result;
}
