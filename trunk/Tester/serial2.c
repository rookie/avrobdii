#include "serial.h"

void uart1_init(UDWORD baud)
{
  UWORD temp;
   UCSR1B = _BV(TXEN1) | _BV(RXEN1);                                      /* tx enable */ 

   if (baud>9600)
   {
   		temp = (FCLK_IO / (8 * baud)) - 1;                   /* 9600 Bd */ 
   		UBRR1H = (UWORD)(temp >> 8); 
   		UBRR1L = (UWORD)temp; 
		BIT_SET(UCSR1A,U2X1);
   		UCSR1C = (3<<UCSZ10);
   }else
   {
   		temp = (FCLK_IO / (16 * baud)) - 1;                   /* 9600 Bd */ 
   		UBRR1H = (UWORD)(temp >> 8); 
   		UBRR1L = (UWORD)temp; 
		BIT_CLEAR(UCSR1A,U2X1);
   		UCSR1C = (3<<UCSZ10);
   }



  /* initialize TWI clock: 100 kHz clock, TWPS = 0 => prescaler = 1 */
  //TWBR = (UBYTE)((SYSCLK / 100000UL - 16) / 2);
}

/*
 * Note [4]
 * Send character c down the UART Tx, wait until tx holding register
 * is empty.
 */
int uart1_putchar(char c)
{

  if (c == '\n')
    uart_putchar('\r');
		
  //while(bit_is_clear(UCSRA, UDRE));
  loop_until_bit_is_set(UCSR1A, UDRE1); 
  UDR1 = c;
  return 0;
}

UBYTE uart1_kbhit(void)
{
	return (UCSR1A & (1<<RXC1));
}

UBYTE uart1_getchar(void)
{
	
	loop_until_bit_is_set(UCSR1A, RXC1); 
	return UDR1;
}

