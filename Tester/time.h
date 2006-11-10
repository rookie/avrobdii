 /*******************************************************************
 *
 *  DESCRIPTION:
 *
 *  AUTHOR : Trampas Stern 
 *
 *  DATE : 5/12/2005  9:20:56 AM
 *
 *  FILENAME: time.h
 *	
 *         Copyright 2004 (c) by Trampas Stern
 *******************************************************************/
#ifndef __TIME_H
#define __TIME_H

#include "system.h"
   

#define TIMER_MILLISEC 2	//this is resoultion of time function
#define TIMER_PRESCALE 256	//prescaller to use (see data sheet)

//this structure holds our time
typedef struct {
	UINT32 seconds;		//seconds since power on
	UINT16 milliSec;   //milliseconds (0-999)
} TIME; 


// ******* Prototypes *********
UINT8 TimeInit(void); //initlizes time
UINT8 GetTime(TIME *ptr); //gets current "time"
UINT32 GetElaspMs(TIME *pStart); //calculates and returns elsaped time
UINT32 GetElaspSec(TIME *pStart); //calculates and returns elsaped time

#endif //__TIME_H

