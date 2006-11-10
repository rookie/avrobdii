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

#ifndef __OBDII_H
#define __OBDII_H

#include "datatypes.h"
#include "checksum.h"
   
//what type of OBDII interface we have
#define UNKNOWN 0
#define VPW 1
#define PWM 2
#define ISO 3

#define SIZE_RESP_STR 60
#define MAX_RESP 10 //maximum ECM responses
 
#define OBDII_PERCENT(x) ((FLOAT)x*(100.0/255.0))
#define OBDII_TEMP(x) ((FLOAT)x-40.0)
#define OBDII_O2VOLTS(x) ((FLOAT)x*0.005)
#define OBDII_O2PERCENT(x) (((FLOAT)x-128.0)*100.0/128.0)

#define NUM_STATUS_LINES 12
typedef struct
{
	UBYTE addr; // ecm address responding
	UBYTE n;	//number of data elements
	UBYTE data[20]; //data response
} OBDII;



UBYTE crc(UBYTE *data, UBYTE len);
UBYTE check_errors(UBYTE *data, UBYTE len, UBYTE connection);
UBYTE process(OBDII *resp, UBYTE num_resp, UBYTE *data, UBYTE num_data, UBYTE connection);
UBYTE obdII_send(UBYTE *msg, UBYTE len_msg, OBDII *resp, UBYTE num_resp);
UBYTE obdII_codes();
UBYTE obdII_clear_codes();

UBYTE obdII_pid_test();
UBYTE obdII_read_PID(UBYTE PID, UBYTE *str, UBYTE nStr,UBYTE *str2, UBYTE nStr2);
UBYTE obdII_run(BYTE index,UBYTE reset);
UBYTE obdII_get_num_pids();

UBYTE obdII_leds_off();

#endif //__OBDII_H
