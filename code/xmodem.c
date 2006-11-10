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

#include "xmodem.h"
#include "delay.h"
#include "serial.h"

#define SOH 0x01
#define STX 0x02
#define ETX 0x03
#define EOT 0x04
#define ENQ 0x05
#define xEOF 0x1A
#define ACK 0x06
#define NAK 0x15
#define ETB 0x17
#define CAN 0x18
#define NUL 0x00

INT Xpacket;
INT Xpacket_comp;
INT expected;
UBYTE Check_sum_cal;
UBYTE Check_sum_recv;

//Internal prototypes
#define  Xmodem_char_avail() uart_kbhit()
#define  Xmodem_get_char() uart_getchar()
#define Xmodem_send_char(data) uart_putchar(data)
UBYTE Xmodem_wait_char(UDWORD timeout);

UINT XmodemUse_CRC=0;
//CRC calculations

UWORD Xmodem_CRC(UBYTE *data)
{
	UWORD i,j;
	UWORD accum;
	UWORD temp;
	accum=0;
	for(i=0; i<XMODEM_PACKET_SIZE; i++)
	{
		temp=data[i]<<8;
		for(j=8; j>0; j--)
		{
			if ((temp ^ accum) & 0x8000)
				accum=(accum<<1) ^XMODEM_CRC_POLY;
			else
				accum<<=1;
			temp<<=1;
		}
	}
	return accum;
}
				 

//void xprint()
//{
//	printf("Last packet =%X ~%X expected %X ,%X\n\r",Xpacket,Xpacket_comp,expected, Xpacket^Xpacket_comp);
//}
//These routines are to recieve data using the XModem protocal

/*******************************************************************
 *  FUNCTION: Xmodem_char_avail
 *  AUTHOR 		= 	TRAMPAS STERN
 *  FILE 		=	xmodem.c
 *  DATE		=   1/26/2003  4:20:53 PM
 *
 *  PARAMETERS: None	
 *
 *  DESCRIPTION: Returns a 1 if a char is available on serial port
 *
 *  RETURNS: 1 char avialable, 0 if not
 *
 *
 *******************************************************************/
/*UBYTE Xmodem_char_avail(void)
{
	return kbhit();
}
*/

/*******************************************************************
 *  FUNCTION: Xmodem_get_char
 *  AUTHOR 		= 	TRAMPAS STERN
 *  FILE 		=	xmodem.c
 *  DATE		=   1/26/2003  4:17:01 PM
 *
 *  PARAMETERS: None
 *
 *  DESCRIPTION: Gets a char from the serial port
 *
 *  RETURNS: char
 *
 *
 *******************************************************************/
/*UBYTE Xmodem_get_char(void)
{
	return getch();
}
*/

/*******************************************************************
 *  FUNCTION: Xmodem_send_char
 *  AUTHOR 		= 	TRAMPAS STERN
 *  FILE 		=	xmodem.c
 *  DATE		=   1/26/2003  4:18:02 PM
 *
 *  PARAMETERS: BYTE to send 
 *
 *  DESCRIPTION:  Sends a char to serial port
 *
 *  RETURNS: 
 *
 *
 *******************************************************************/
/*void Xmodem_send_char(UBYTE data)
{
	putch(data);
}
*/

/*******************************************************************
 *  FUNCTION: Xmodem_wait_char
 *  AUTHOR 		= 	TRAMPAS STERN
 *  FILE 		=	xmodem.c
 *  DATE		=   1/26/2003  4:35:34 PM
 *
 *  PARAMETERS:  UBYTE for millisecond time out
 *
 *  DESCRIPTION: waits time out period for a char
 *
 *  RETURNS: 0 char available, 1 Timeout
 *
 *
 *******************************************************************/

UBYTE Xmodem_wait_char(UDWORD timeout)
{
   	int i;
   	if (Xmodem_char_avail()!=0)
			return 0;
   	while (timeout--)
	{
		for(i=0; i<10; i++)
		{
			if (Xmodem_char_avail()!=0)
				return 0;
			delay_us(100);	//delay 1 ms
		} 

		ClrWdt(); 	//Restart watch dog timer
	}
	return 1;
}

   	




UBYTE Xmodem_get_packet2(UBYTE *data, UBYTE packet_num)
{
	UBYTE i=0;
	UBYTE temp=0;
	UBYTE temp2=0;

    //BmpDisabled(); //disable screen capture
    
  	Xpacket=0x00;
  	Xpacket_comp=0x00;
  	
  	packet_num=packet_num & 0x00FF; //truncate to 8 bits
  	
	//Now we need to wait for SOH char on serial port
	if (Xmodem_wait_char(SOH_TIME_OUT))			
		return 	XERROR_SOH;
	
	//check to see if we got a char
	temp=Xmodem_get_char();

	//Check to see if we got a EOT
	if (temp==EOT)
	{
		Xmodem_send_char(ACK);	
		return RECV_EOT;
	}

	//Check for start of packet
	if (temp!=SOH)
		return XERROR_SOH; 

	//Now we have received the SOH lets get the packet number
	if (Xmodem_wait_char(CHAR_TIME_OUT))
		return 	XERROR_NO_CHAR | XERROR_IN_PACKET;
  	temp=Xmodem_get_char();	    //check to see if we got a char
	Xpacket=temp;

	//get complement of packet number
	if (Xmodem_wait_char(CHAR_TIME_OUT))
		return 	XERROR_NO_CHAR | XERROR_IN_PACKET;
	temp2=Xmodem_get_char();
	Xpacket_comp=temp2;

	 expected=packet_num;
	//check to see if we are getting the expected packet
	if ((temp!=packet_num))
		return XERROR_WRONG_PACKET;

	//check to see that our complement is correct
	if ((temp^temp2)!=0xFF)
		return XERROR_WRONG_COMP;

	//OK we can receive a packet of data
	Check_sum_cal=0;								//use temp2 as our check sum counter
	for(i=0; i<XMODEM_PACKET_SIZE; i++)
	{
		//wait for a char
		if (Xmodem_wait_char(CHAR_TIME_OUT))
			return 	XERROR_NO_CHAR | XERROR_IN_DATA;
	
		data[i]= Xmodem_get_char();			//Get the char and save it into buffer
		Check_sum_cal=Check_sum_cal+data[i]; 				//add to check sum
	}

	//Get the Check Sum 
	if (Xmodem_wait_char(CHAR_TIME_OUT))
		return 	XERROR_NO_CHAR|XERROR_IN_CHECK;
	Check_sum_recv=Xmodem_get_char();

	if ((Check_sum_recv & 0x0FF)!=(Check_sum_cal & 0x0FF))
		return XERROR_CHECKSUM;
	
	//BmpEnabled();
	return NO_ERRORS;
}

/*******************************************************************
 *  FUNCTION: Xmodem_get_packet
 *  AUTHOR 		= 	TRAMPAS STERN
 *  FILE 		=	xmodem.c
 *  DATE		=   1/26/2003  4:06:02 PM
 *
 *  PARAMETERS: pointer to 128 byte array where to put data
 *
 *  DESCRIPTION: This function gets data from a host using xmodme
 *
 *  RETURNS: 0 pass,
 *			99- Unexpected error 
 *			1-SOH time out
 *
 *
 *
 *******************************************************************/
UBYTE Xmodem_get_packet(UBYTE *data, UBYTE packet_num)
{
	UWORD ret,k;
	k=0;
	ret=1;

	ret=Xmodem_get_packet2(data,packet_num);
	return ret;

	while(ret!=0 && k<30)
	{
		ret=Xmodem_get_packet2(data,packet_num);
		if (ret==0 || ret==RECV_EOT)
			return ret;
					
		Xmodem_send_char(NAK); //send NAK
		delay_ms(2); //give host time to reload packet
		
		//flush receive buffer
		while(Xmodem_char_avail())
				Xmodem_get_char();
   }
   	
	//well we good or bad
	return ret;
}
	
		
/*******************************************************************
 *  FUNCTION: Xmodem_send_cancel
 *  AUTHOR 		= 	TRAMPAS STERN
 *  FILE 		=	xmodem.c
 *  DATE		=   1/26/2003  4:56:39 PM
 *
 *  PARAMETERS: none	
 *
 *  DESCRIPTION: Sends a cancel to host to end transmission
 *
 *  RETURNS: nothing
 *
 *
 *******************************************************************/
void Xmodem_send_cancel(void)
{
	Xmodem_send_char(CAN);
}

/*******************************************************************
 *  FUNCTION: Xmodem_send_retry
 *  AUTHOR 		= 	TRAMPAS STERN
 *  FILE 		=	xmodem.c
 *  DATE		=   1/26/2003  4:58:06 PM
 *
 *  PARAMETERS: None
 *
 *  DESCRIPTION: Sends a request to retry packet
 *
 *  RETURNS: void
 *
 *
 *******************************************************************/
void Xmodem_start(void)
{		 
	Xmodem_send_char(NAK);
}


UINT Xmodem_wait_TX()
{
	UBYTE temp;
	UINT done;

	done=0;

	while(done<100)
	{
		//Now we need to wait fo ACK 
		if (Xmodem_wait_char(NAK_TIME_OUT))
		{
			log_printf("WARNING: Xmodem tx time out");
			return 	XERROR_NAK_TIMEOUT;	 //error
		}
		
		//check to see if we got a char
		temp=Xmodem_get_char();	
		
		if (temp=='C')
		{
			XmodemUse_CRC=1;
			return NO_ERRORS;
		}
		if (temp==CAN)
		{
			return XERROR_RCV_QUIT;
		}
		if (temp==NAK)
			return NO_ERRORS;
			
		log_printf("WARNING: Xmodem tx bad char");
		done++;
	}
	return 0;
}


UINT Xmodem_put_packet(UBYTE *data, UBYTE packet_num)
{
	UBYTE pack_comp;
	UBYTE checksum;
	UWORD crc_value;
	UBYTE temp;
	UBYTE i;
	UBYTE retry;
	
	//BmpDisabled(); //disable screen capture
	temp=NAK;
	retry=0;
	while (temp==NAK && retry<200)
	{
		packet_num=packet_num & 0x00FF; //truncate to 8 bits
	  	
		//calculate the packet complement
		pack_comp=((~packet_num) & 0x00FF);
		
		crc_value=Xmodem_CRC(data);
		//send the packet
		Xmodem_send_char(SOH);
		Xmodem_send_char(packet_num);
		Xmodem_send_char(pack_comp);
		checksum=0;
		for(i=0; i<XMODEM_PACKET_SIZE; i++)
		{
			//send a char
			Xmodem_send_char(data[i]);

			//add to check sum
			checksum=(checksum+data[i]) & 0x00FF; 
		}
		
		if (XmodemUse_CRC==1)
		{
			Xmodem_send_char(crc_value>>8);
			Xmodem_send_char(crc_value & 0x00FF);
		}else
		{
			//send checksum
			Xmodem_send_char(checksum);
		}
			
		//Now we need to wait fo ACK 
		if (Xmodem_wait_char(ACK_TIME_OUT))
		{
			Xmodem_send_char(CAN);
			//BmpEnabled();
			return 	XERROR_ACK_TIME;	 //error
		}
		
		//check to see if we got a char
		temp=Xmodem_get_char();
		
		if (temp==CAN)
		{
			//BmpEnabled();
			return XERROR_RCV_QUIT;
		}
		
		if (temp==ACK)
		{
			//BmpEnabled();
			return NO_ERRORS;
		}
	
		retry++;	
	}
	
	///BmpEnabled();
	return XERROR_TX_ERROR; //unknown error
	
}

UINT Xmodem_send_EOT()
{
    Xmodem_send_char(EOT);
    //BmpEnabled();
    return NO_ERRORS;
}			
	
	
	
/*******************************************************************
 *  FUNCTION: Xmodem_send_ack
 *  AUTHOR 		= 	TRAMPAS STERN
 *  FILE 		=	xmodem.c
 *  DATE		=   1/27/2003  3:54:12 PM
 *
 *  PARAMETERS: none
 *
 *  DESCRIPTION: Send acknologement of last packet
 *
 *  RETURNS: void
 *
 *
 *******************************************************************/
void Xmodem_send_next(void)
{
	Xmodem_send_char(ACK);
}

void Xmodem_debug()
{
	printf("\n\rExpected packet =%d\n\r",expected);
	printf("Host sent packet = %d\n\r",Xpacket);
	printf("Host compilment packet = %d\n\r",Xpacket_comp);
	printf("Calculated check sum = %X\n\r",Check_sum_cal);
	printf("Recieved check sum =%X\n\r",Check_sum_recv);
}

	
