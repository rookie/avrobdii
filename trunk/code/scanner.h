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

 
