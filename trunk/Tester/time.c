/*******************************************************************
 *
 *  DESCRIPTION:
 *
 *  AUTHOR : Trampas Stern 
 *
 *  DATE : 5/12/2005  9:48:19 AM
 *
 *  FILENAME: time.c
 *	
 *         Copyright 2004 (c) by Trampas Stern
 *******************************************************************/
#include "time.h"

UINT32 Seconds=0;
UINT16 MilliSeconds=0;

/*******************************************************************
 *  FUNCTION: TimeInit
 *  AUTHOR 		= 	TRAMPAS STERN
 *  FILE 		=	time.c	
 *  DATE		=   5/12/2005  9:48:35 AM
 *
 *  PARAMETERS: 	
 *
 *  DESCRIPTION: Sets up the ISR for the timer 
 *
 *  RETURNS: 
 *
 *               Copyright 2004 (c) by Trampas Stern
 *******************************************************************/
UINT8 TimeInit(void)
{
	UINT8 count;

	//we need to calculate the count value
	count=(UINT8)((FCLK_IO/TIMER_PRESCALE)/(1.0/(.001*TIMER_MILLISEC)));

#if  TIMER_PRESCALE==1
	TCCR0A=0x01;
#endif 
#if  TIMER_PRESCALE==8
	TCCR0A=0x02;
#endif 
#if  TIMER_PRESCALE==64
	TCCR0A=0x03;
#endif 
#if  TIMER_PRESCALE==256
	TCCR0A=0x04;
#endif 
#if  TIMER_PRESCALE==1024
	TCCR0A=0x05;
#endif
	TCCR0A=TCCR0A |= 0x08; //enable Clear Timer on Compare
	OCR0A=count; //set count 
	TIMSK0=0x02; //enable ISR on compare match. 

    return NO_ERROR;
}
/*******************************************************************
 *  FUNCTION: TimerISR
 *  AUTHOR 		= 	TRAMPAS STERN
 *  FILE 		=	time.c	
 *  DATE		=   5/12/2005  10:14:45 AM
 *
 *  PARAMETERS: 	
 *
 *  DESCRIPTION:  This ISR is executed when timer0 overflows
 *
 *  RETURNS: 
 *
 *               Copyright 2004 (c) by Trampas Stern
 *******************************************************************/
SIGNAL (SIG_OUTPUT_COMPARE0)
{
	MilliSeconds+=TIMER_MILLISEC;
	if (MilliSeconds>=1000)
	{
		Seconds++;
		MilliSeconds=0;
	}
}

/*******************************************************************
 *  FUNCTION: GetTime
 *  AUTHOR 		= 	TRAMPAS STERN
 *  FILE 		=	time.c	
 *  DATE		=   5/12/2005  10:16:50 AM
 *
 *  PARAMETERS: 	
 *
 *  DESCRIPTION: Gets the current time
 *
 *  RETURNS: 
 *
 *               Copyright 2004 (c) by Trampas Stern
 *******************************************************************/
UINT8 GetTime(TIME *ptr)
{
	//first we need to disable the Timer ISR 	
	TIMSK0=TIMSK0 & ~(1<<OCIE0A); //Turn Timer0 compare A interrupt off

	//set the time variables
	ptr->milliSec=MilliSeconds;
	ptr->seconds=Seconds;

	//turn ISR back on
	TIMSK0=TIMSK0 | (1<<OCIE0A); //Turn Timer0 compare A interrupt ON

	return NO_ERROR;
}

/*******************************************************************
 *  FUNCTION: GetElaspMs
 *  AUTHOR 		= 	TRAMPAS STERN
 *  FILE 		=	time.c	
 *  DATE		=   5/12/2005  10:19:48 AM
 *
 *  PARAMETERS: pointer to Time of start 	
 *
 *  DESCRIPTION: Returns number of milliseconds elasped
 *
 *  RETURNS: 
 *
 *               Copyright 2004 (c) by Trampas Stern
 *******************************************************************/
UINT32 GetElaspMs(TIME *pStart)
{
	UINT32 temp;
	TIME t;
	GetTime(&t);

	temp=(t.seconds-pStart->seconds)*1000;
  	temp=temp + t.milliSec;
  	temp=temp-pStart->milliSec;
	return temp;
}

/*******************************************************************
 *  FUNCTION: GetElaspSec
 *  AUTHOR 		= 	TRAMPAS STERN
 *  FILE 		=	time.c	
 *  DATE		=   5/12/2005  10:29:25 AM
 *
 *  PARAMETERS:   pointer to Time of start	
 *
 *  DESCRIPTION: returns elasped seconds
 *
 *  RETURNS: 
 *
 *               Copyright 2004 (c) by Trampas Stern
 *******************************************************************/
UINT32 GetElaspSec(TIME *pStart)
{
	UINT32 temp;
	TIME t;
	GetTime(&t);

	temp=(t.seconds-pStart->seconds);
 	return temp;
}


