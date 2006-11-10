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

#include "serial.h"

void uart_init(UDWORD baud)
{
  UWORD temp;
   UCSR0B = _BV(TXEN0) | _BV(RXEN0);                                      /* tx enable */ 

   if (baud>9600)
   {
   		temp = (FCLK_IO / (8 * baud)) - 1;                   /* 9600 Bd */ 
   		UBRR0H = (UWORD)(temp >> 8); 
   		UBRR0L = (UWORD)temp; 
		BIT_SET(UCSR0A,U2X0);
   		UCSR0C = (3<<UCSZ00);
   }else
   {
   		temp = (FCLK_IO / (16 * baud)) - 1;                   /* 9600 Bd */ 
   		UBRR0H = (UWORD)(temp >> 8); 
   		UBRR0L = (UWORD)temp; 
		BIT_CLEAR(UCSR0A,U2X0);
   		UCSR0C = (3<<UCSZ00);
   }



  /* initialize TWI clock: 100 kHz clock, TWPS = 0 => prescaler = 1 */
  //TWBR = (UBYTE)((SYSCLK / 100000UL - 16) / 2);
}

/*
 * Note [4]
 * Send character c down the UART Tx, wait until tx holding register
 * is empty.
 */
int uart_putchar(char c)
{

  if (c == '\n')
    uart_putchar('\r');
		
  //while(bit_is_clear(UCSRA, UDRE));
  loop_until_bit_is_set(UCSR0A, UDRE0); 
  UDR0 = c;
  return 0;
}

UBYTE uart_kbhit(void)
{
	return (UCSR0A & (1<<RXC0));
}

UBYTE uart_getchar(void)
{
	
	loop_until_bit_is_set(UCSR0A, RXC0); 
	return UDR0;
}

