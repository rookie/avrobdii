
#include "system.h"
#include "delay.h"
#include "serial.h"
#include "serial2.h"
#include "time.h"


#define L_OUT_DDR DDRC
#define L_OUT_PORT PORTC
#define L_OUT_PIN  1
//invert the logic such that we set based on interface level
#define L_OUT(x) {if(!x) BIT_SET(L_OUT_PORT,L_OUT_PIN); else BIT_CLEAR(L_OUT_PORT,L_OUT_PIN);}

#define K_OUT_DDR DDRD
#define K_OUT_PORT PORTD
#define K_OUT_PIN  3
//invert the logic such that we set based on interface level
#define K_OUT(x) {if(x) BIT_SET(K_OUT_PORT,K_OUT_PIN); else BIT_CLEAR(K_OUT_PORT,K_OUT_PIN);}

#define L_IN BIT_TEST(PINE,7)
#define K_IN BIT_TEST(PIND,2)


int main(void)
{
	UINT8 data;

	BIT_SET(L_OUT_DDR,L_OUT_PIN); //L_OUT pin is output
	BIT_SET(K_OUT_DDR,K_OUT_PIN); //K_OUT pin is output

	K_OUT(1);
	L_OUT(1);


	uart1_init(10400);
	uart_init(115200); //Set up UART
	TimeInit();	 //set up time
	
	//set up printf
	fdevopen(uart_putchar, NULL);

	printf("Hello world\n"); 
   
   	while (1)
	{
		if (uart1_kbhit())
		{
			data=uart1_getchar();
			printf("0x%02X ",data);
			//uart_putchar(data);
		}
		if (uart_kbhit())
		{
			data=uart_getchar();
			uart1_putchar(data);
		}
	}
			
		 
   		
}
