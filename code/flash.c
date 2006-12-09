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
 *	File: flash.c
 *
 * 	Copyright ©,  Trampas Stern. All Rights Reserved.	  
 *  Date: 7/23/2006	10:08:52 AM
 *******************************************************************/

#include "flash.h"
#include "delay.h"
#include "xmodem.h"
#include "serial.h"

UINT8 spi_write(UINT8 data);
void flash_wait();

#define FLASH_CS_DDR DDRB
#define FLASH_CS_PORT PORTB
#define FLASH_CS_BIT 0
#define FLASH_CS(x) {if(x) BIT_SET(FLASH_CS_PORT,FLASH_CS_BIT); else BIT_CLEAR(FLASH_CS_PORT,FLASH_CS_BIT);}

#define FLASH_RESET_DDR DDRD
#define FLASH_RESET_PORT PORTD
#define FLASH_RESET_PIN 7
#define FLASH_RESET(x) {if(x) BIT_SET(FLASH_RESET_PORT,FLASH_RESET_PIN); else BIT_CLEAR(FLASH_RESET_PORT,FLASH_RESET_PIN);}


#define FLASH_CLK(x) {if(x) BIT_SET(PORTB,1); else BIT_CLEAR(PORTB,1);}
#define FLASH_MOSI(x) {if(x) BIT_SET(PORTB,2); else BIT_CLEAR(PORTB,2);}
#define FLASH_MISO BIT_TEST(PINB,3)


#define FLASH_DELAY 10


/******************************************************************
 ** flash_init
 *
 *  DESCRIPTION:
 *
 *  Create: 7/23/2006	10:09:14 AM - Trampas Stern
 *******************************************************************/
void flash_init()
{
	//Set the Chip Select pin as output
	BIT_SET(FLASH_CS_DDR,FLASH_CS_BIT); 
	//SET Chip select high
	FLASH_CS(1); 
	FLASH_CLK(0);

	BIT_SET(FLASH_RESET_DDR,FLASH_RESET_PIN);
	FLASH_RESET(0);
	delay_ms(20);
	FLASH_RESET(1);

	//Set up the SPI port
	// AT45DBxxx can handle mode 0 or mode 1 transmissions
	// we will use mode zero (SCK low when idle & sample leading edge)
	//	SPI Interupt disabled - 0
	//	SPI Disabled - 0
	// 	DORD MSB first -0
	//  Master - 1
	//  Clk Polarity - 0 SCK low when idel
	//	CPHA =0 sample leading edge
	//  ClkRate= 000 (AT90 is 16Mhz and Flash can operate at 20Mhz, no clk divide)
	SPCR=0x10; //only master bit set

	//configure the direction of the SPI pins
	BIT_SET(DDRB,1); //SCK output
	BIT_SET(DDRB,2); //MOSI output

	//SET MISO to have internal pullup
	BIT_SET(PORTB,3); 

	//Enable the SPI port
	SPCR=SPCR | 0x40; 

}

/******************************************************************
 ** SPI_write
 *
 *  DESCRIPTION:
 *		Writes a byte of data to the SPI port and returns the result
 *
 *  Create: 7/23/2006	1:42:15 PM - Trampas Stern
 *******************************************************************/

UINT8 spi_write(UINT8 data)
{
	UINT8 temp;
	//Clear the SPI status
	SPSR=0xC0;
	temp=SPDR; //dummy read to clear SPI status

	//Now transfer data to SPI Data register
	SPDR=data;
	while ((SPSR & 0xC0)==0)
	{
		//TO DO add some time outs
	}
	//Clear the SPI status
	SPSR=0xC0;
	temp=SPDR; //dummy read to clear SPI status
	return temp;
}


/*

UBYTE spi_write(UBYTE data)
{
	UBYTE ret;	
	UBYTE mask;
	//assume CS is driven low
	ret=0;
	mask=0x80;
	while(mask)
	{
		ret=ret>>1;
		
		FLASH_CLK(0);
		delay_ms(FLASH_DELAY);
		FLASH_MOSI((data & mask)>0);
		FLASH_CLK(1);
		if (FLASH_MISO)
			ret=ret | 0x80;
		delay_ms(FLASH_DELAY);
		mask=mask>>1;
	}
	FLASH_CLK(0);
	return ret;
}
	
*/		


//Erases all of flash memory
UBYTE flash_erase_page(UWORD page) 
{
	UWORD temp;

	//first send the erase opcode;
	FLASH_CS(0);
	spi_write(0x81);
	temp=page<<1;;
	spi_write((temp>>8) & 0xff);
	spi_write(temp & 0xff);
	spi_write(0x00);
	FLASH_CS(1);	

	delay_us(10);
	//now lets wait for the status to say all is good.
	flash_wait();
	return 0; //no error
}	

UBYTE flash_read(UDWORD addr)
{
    UBYTE buff_addr;
	UDWORD page;
	
	//make address correct
	buff_addr=addr & 0xFF;
	page=addr & FLASH_MAX_PAGES; 
	page=page<<1;
	
	//printf_P(PSTR("addr %lX page %lX %X\n\r",addr,page,buff_addr);
	//send read command
	flash_wait();
	FLASH_CS(0);
	spi_write(0xD2);
	spi_write(MAKE8(page,2));
	spi_write(MAKE8(page,1));
	spi_write(buff_addr);
	//send 4 don't care bytes
	spi_write(0);
	spi_write(0);
	spi_write(0);
	spi_write(0);
	buff_addr=spi_write(0);
	FLASH_CS(1);
	return buff_addr;
}

void flash_wait()
{
	UBYTE status;
	UWORD timeout;

	FLASH_CS(0);
	spi_write(FLASH_RD_STATUS);
	status=spi_write(0);
	timeout=0;
	while ((status & 0x80)==0)
	{
		status=spi_write(0);
		timeout++;
		delay_us(2);
		if(timeout>10000)
		{
			printf_P(PSTR("ERROR: flash time out %X\n\r"),status);
			return;
		}
		//printf_P(PSTR("status %X\n\r",status);
	} 
	FLASH_CS(1);
}

UBYTE flashRead(UBYTE *data, UBYTE count, UDWORD address)
{
	UBYTE i;
	UDWORD addr;
	addr=address;
	
	for(i=0; i<count; i++)
	{
		data[i]=flash_read(addr);
		addr++;
	}
	return i;
}

UBYTE flash_buf_read(UDWORD address)
{
	UBYTE buff_addr;
	UDWORD page;
	
	//make address correct
	buff_addr=MAKE8(address,0);
	page=address & FLASH_MAX_PAGES; 
	page=page<<1;
	//addr=addr | buff_addr;
	
	//read page into buffer
	//transfer main memory into buffer #1
	FLASH_CS(0);
	spi_write(0xD4);
	spi_write(0);
	spi_write(0);
	spi_write(buff_addr);
	spi_write(0);
	buff_addr=spi_write(0);
	FLASH_CS(1);	
	return buff_addr;
}
	

UBYTE flash_put(UDWORD address, UBYTE data)
{
	UBYTE buff_addr;
	UDWORD page;
	
	//make address correct
	buff_addr=address & 0xFF;
	page=address & FLASH_MAX_PAGES; 
	page=page<<1;
	//addr=addr | buff_addr;


	//read page into buffer
	//transfer main memory into buffer #1
	FLASH_CS(0);  //drive chip select low
	spi_write(0x53);
	spi_write(MAKE8(page,2));
	spi_write(MAKE8(page,1));
	spi_write(buff_addr);
	FLASH_CS(1);

    flash_wait();

	//write byte to buffer
	FLASH_CS(0);  //drive chip select low
	spi_write(0x84);
	spi_write(0);
	spi_write(0);
	spi_write(buff_addr);
	spi_write(data);
	FLASH_CS(1);
	delay_ms(1);
	flash_wait();

	//we need to write buffer to page
	FLASH_CS(0);  //drive chip select low
	spi_write(0x83);
	spi_write((page>>16) & 0xFF);
	spi_write((page>>8) & 0xFF);
	spi_write((page) & 0xFF);
	FLASH_CS(1);

	delay_ms(20);

	flash_wait();
  


	return 0;
}


//erase all of flash
UBYTE flash_erase()
{
	UWORD i;
//	UBYTE temp[20];
//	sprintf_P(temp,"Eraseing");
//	LCD_print1(temp,0);
	for(i=0; i<(FLASH_MAX_PAGES>>8); i++)
	{
		//sprintf_P(temp,"%lu",i);
		//LCD_print2(temp,2);
		printf_P(PSTR("Erasing Page %u\n\r"),i);
		if (flash_erase_page(i)!=0)
			return 1;
	}
//	sprintf_P(temp,"DONE!");
//	LCD_print2(temp,0);
	return 0;
}

/*******************************************************************
 *  FUNCTION: Flash_serial_program
 *  AUTHOR 		= 	TRAMPAS STERN
 *  FILE 		=	flash.c
 *  DATE		=   1/26/2003  5:17:39 PM
 *
 *  PARAMETERS: Destination address
 *
 *  DESCRIPTION: gets xmodem packet and programs to flash
 *
 *  RETURNS: 	0 - pass,
 *				1 - Flash write failed
 *				
 *
 *
 *******************************************************************/
UBYTE Flash_serial_program(UDWORD address)
{
    UWORD i;
    UBYTE data[XMODEM_PACKET_SIZE];
	UBYTE ret;
	UBYTE done;
	UBYTE packet;
	UBYTE buff_addr;
	UDWORD page;
	UDWORD bytes;

	packet=1;		//first packet is 1
    bytes=0;	 

	//make address correct
	buff_addr=MAKE8(address,0);
	page=address & FLASH_MAX_PAGES; 
	page=page<<1;
	//addr=addr | buff_addr;


	//read page into buffer
	//transfer main memory into buffer #1
	FLASH_CS(1);
	spi_write(0x53);
	spi_write(MAKE8(page,2));
	spi_write(MAKE8(page,1));
	spi_write(buff_addr);
	FLASH_CS(1);
	flash_wait();

	//send request to start transimission
	// we will retry 40 times for slow users
	i=0;
	done=0;
	do {
		Xmodem_start();
		ret=Xmodem_get_packet(data,packet);	  //Get a packet of information	
	}while(ret==XERROR_SOH && i++<300);
	   		
	while(!done)
	{				
		UBYTE i;
		//then we need to program the data into flash
		for(i=0; i<XMODEM_PACKET_SIZE; i++)
		{
			//write each byte to buffer
			FLASH_CS(0);
			spi_write(0x84);
			spi_write(0);
			spi_write(0);
			spi_write(buff_addr);
			spi_write(data[i]);
			FLASH_CS(1);
			flash_wait();

 
			buff_addr++;
			//if buffer full write buffer to page 
			// and read next buffer
			if (buff_addr==0x00)
			{
				//we need to write buffer to page
				FLASH_CS(0);
				spi_write(0x83);
				spi_write(MAKE8(page,2));
				spi_write(MAKE8(page,1));
				spi_write(buff_addr);
				FLASH_CS(1);
				delay_ms(20);
				flash_wait();


				//read next page into buffer
				page=page+0x00200;
				FLASH_CS(0);
				spi_write(0x53);
				spi_write(MAKE8(page,2));
				spi_write(MAKE8(page,1));
				spi_write(buff_addr);
				FLASH_CS(1);
				flash_wait();

			}
			bytes++;
		}	
		packet++;
		//send request for next packet 
		if (ret==NO_ERRORS)
		{
			Xmodem_send_next();
			ret=Xmodem_get_packet(data,packet);	  //Get a packet of information
		}else 
		{
		  Xmodem_goodbye();	
		  done=1;
		}
			

	};

	//we need to write buffer to page
	FLASH_CS(0);
	spi_write(0x83);
	spi_write(MAKE8(page,2));
	spi_write(MAKE8(page,1));
	spi_write(buff_addr);
	FLASH_CS(1);
	delay_ms(20);
	flash_wait();

	if (ret==NO_ERRORS || ret==RECV_EOT)
	{
		Xmodem_goodbye();	
		delay_ms(500);
		printf_P(PSTR("\n\rFlashed %lu Bytes\n\r"),bytes);
		printf_P(PSTR("Flash Program completed\n\r"));
		return NO_ERRORS;
	}
   	
	Xmodem_send_cancel();	//tell host to cancel
	delay_ms(1000); //wait for host to give up
	while(uart_kbhit())
		uart_getchar();
	printf_P(PSTR("Flash Program ERROR %X packet=%d\n\r"),ret,packet);
	//printf_P(PSTR("Packet expect %d, got %d comp %d\n\r",expected,Xpacket,Xpacket_comp);
	printf_P(PSTR("Press x to exit\n\r"));
	while( uart_getchar()!='x'); 
   	return ret;				//else lets return the xmodem error
}
