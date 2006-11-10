/*******************************************************************
 *	File: scanner.h
 *
 * 	Copyright ©,  Trampas Stern. All Rights Reserved.	  
 *  Date: 5/9/2006	9:01:40 PM
 *******************************************************************/
#ifndef __SCANNER_H
#define __SCANNER_H

//This file is for macros which will be used through out project

#include "system.h"

#define USE_LCD 1

//write data out "data" bus
#define DATA_WRITE(x) { \
		DDRA=0xFF; \
		PORTA=x; \
		}




#endif //__SCANNER_H

 
