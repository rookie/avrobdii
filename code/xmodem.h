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

#ifndef __X_MODEM_H_
#define __X_MODEM_H_

#include "datatypes.h"

#define XMODEM_PACKET_SIZE ((UWORD)128)
#define XMODEM_CRC_POLY	 0x1021

#define SOH_TIME_OUT  10000 	//Time out in milliseconds to wait for start of header
#define CHAR_TIME_OUT 30000		//Time out in milliseconds to wait for a char

//Transmitter ack/NAC time outs
#define ACK_TIME_OUT 300000
#define NAK_TIME_OUT 600000

//Main xmodem receive funcion
UBYTE Xmodem_get_packet(UBYTE *data, UBYTE packet_num);

//main xmodem transmit function
UINT Xmodem_put_packet(UBYTE *data, UBYTE packet_num);
UINT Xmodem_wait_NAK();
UINT Xmodem_send_EOT();

//Return Values
#define NO_ERRORS			0	//No errors
#define RECV_EOT			11 	//REceived EOT transmission Good thing
#define XERROR_SOH			2	//Failed to recieve SOH or NAK
#define XERROR_NO_CHAR  	3	//No CHAR recieved in time out period
#define XERROR_WRONG_PACKET 4	//Recieved wrong packet or bad packet number
#define XERROR_WRONG_COMP   5	//REcieved wrong packet complement
#define XERROR_CHECKSUM 	6	//Bad check sum
#define XERROR_RCV_QUIT	    7	//reciver quit
#define XERROR_ACK_TIME		8	//transmitter ack timeout
#define XERROR_TX_ERROR		9	//unknown tx error
#define XERROR_NAK_TIMEOUT  0x0a	
#define XERROR_IN_PACKET 	0x10	//during packet numbers
#define XERROR_IN_DATA		0x20
#define XERROR_IN_CHECK		0x30

//alias function for good bye
#define Xmodem_goodbye 	Xmodem_send_next

//extern BYTE Xpacket;
//extern BYTE Xpacket_comp;
//extern BYTE expected;

void Xmodem_send_cancel(void);
void Xmodem_start(void);
void Xmodem_send_next(void);

//print out some debug to serial port, useful when xmodem fails
void Xmodem_debug();

//the following are for xmodem to host
UINT Xmodem_send_EOT();
UINT Xmodem_put_packet(UBYTE *data, UBYTE packet_num);
UINT Xmodem_wait_TX();

#endif
