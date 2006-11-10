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
 *	File: J1850.h
 *
 * 	Copyright ©,  Trampas Stern. All Rights Reserved.	  
 *  Date: 5/19/2006	9:31:34 PM
 *******************************************************************/
#ifndef __J1850_H
#define __J1850_H

#include "scanner.h"

INT J1850Init(void);
INT J1850test(void);
INT pwm_get(UINT8 *ptr, UINT8 count, UINT16 time_out_ms);
INT pwm_put(UINT8 *data, UINT8 count, UINT16 time_out_ms);
UINT8 crc(UINT8 *data, UINT8 len);

#endif //__J1850_H

