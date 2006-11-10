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

