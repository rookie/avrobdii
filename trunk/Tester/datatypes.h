/*************************************
Datatypes.h 

11/10/2004 Trampas Stern
*************************************/

#ifndef __DATATYPES_H
#define __DATATYPES_H

#include <inttypes.h>

typedef unsigned char UINT;
typedef signed char INT;
typedef char CHAR;
typedef unsigned int UWORD;
typedef signed char BYTE;
typedef unsigned char UBYTE;
typedef signed int WORD;
typedef unsigned long UDWORD;
typedef signed long DWORD; 
typedef float  FLOAT; 


typedef unsigned char UINT8;
typedef signed char INT8;
typedef unsigned int UINT16;
typedef signed int INT16;
typedef unsigned long UINT32;
typedef signed long INT32; 



typedef union {
	UWORD data ;
	struct {
		BYTE low;
		BYTE high;
	};
} UWORDbytes;

#define MAX_UDWORD 0xFFFFFFFF

#endif //__DATATYPES_H
