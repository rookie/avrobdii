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
 *	File: iso.c
 *
 * 	Copyright ©,  Trampas Stern. All Rights Reserved.	  
 *  Date: 7/23/2006	4:23:08 PM
 *******************************************************************/

#include "iso.h"
#include "time.h"
#include "delay.h"
#include "checksum.h"

#define ISO_BUFFER_SIZE 10

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


//#define TX_PIN(x) {if(txPins==1) K_OUT(x); else if(txPins==2) K_OUT(x); else {K_OUT(x); L_OUT(x);}}
#define TX_PIN(x)  {K_OUT(x);}
#define RX_PIN		K_IN

#define DISABLE_OVF_ISR() {BIT_CLEAR(TIMSK2,0);}
#define ENABLE_OVF_ISR() {BIT_SET(TIMSK2,0);}
#define ENABLE_RX_ISR() {BIT_SET(UCSR1B,7);}
#define DISABLE_RX_ISR() {BIT_CLEAR(UCSR1B,7);}

//we  need state machine inside this file
#define ISO_UNKNOWN	0	//we are disconnected or unknown state
#define ISO_CONNECT 1	//we are connected and baud is correct rate

#define ISO_PRESCALE 8  //Timer prescaler

UBYTE iso_getAuto(UBYTE *data, UBYTE len, UWORD time_out_ms,UBYTE autoBaud );

UWORD baudTicks=0;
UBYTE iso_state=ISO_UNKNOWN;
UBYTE txPins=0;	//1 for K_LINE, 2 for L_LINE, 3 for both

UINT16 Baud;


typedef enum  {
	IDLE=0,	 	//bus idle state
	RX_START,   //start bit recieved
	RX_BITS,	//recieving bits
	RX_STOP, 	//recieving Stop bit
	TX_START, 	//Sending Start bit
	TX_BITS,	//Sending Bits
	TX_STOP,	//Sending stop bit(s)
	COLLISION,  //Collision on bus detected.
	UNKNOWN	//Bus in Unknown state 
}BUS_STATES;

UINT8 RxBuffer[ISO_BUFFER_SIZE];
UINT8 TxBuffer[ISO_BUFFER_SIZE];
//read and write indexes for buffers
volatile UINT8 RxRead=0;
volatile UINT8 RxWrite=0;
volatile UINT8 TxRead=0;
volatile UINT8 TxWrite=0;

volatile UINT8 RxState=UNKNOWN;



/******************************************************************
 ** iso_init
 *
 *  DESCRIPTION:
 *		Sets up the ISO interface 
 *		
 *
 *  Create: 7/23/2006	4:23:14 PM - Trampas Stern
 *******************************************************************/
INT iso_init(UINT32 baud)
{
	UINT32 temp;

	//Set up the data direction registers 
	BIT_SET(L_OUT_DDR,L_OUT_PIN); //L_OUT pin is output
	BIT_SET(K_OUT_DDR,K_OUT_PIN); //K_OUT pin is output

	K_OUT(1);
	L_OUT(1);

	//first we are going to use timer2 for the ISO timing
	//TODO calculate the baud rate
  	baudTicks=0;

  	TCCR2A=0x01; //Turn timer on 
	TCNT2=0;
	//OCR2A=(UINT8)temp;

	//make sure the OCR2A ISR is off
	TIMSK2=0x00;

	//UCSR1B = _BV(TXEN1) | _BV(RXEN1);/* tx/rx enable */ 

	if (baud>9600)
	{
			temp = (FCLK_IO / (8 * baud)) - 1;                    
			UBRR1H = (UWORD)(temp >> 8); 
			UBRR1L = (UWORD)temp; 
		BIT_SET(UCSR1A,U2X1);
			UCSR1C = (3<<UCSZ10);
	}else
	{
			temp = (FCLK_IO / (16 * baud)) - 1;                    
			UBRR1H = (UWORD)(temp >> 8); 
			UBRR1L = (UWORD)temp; 
		BIT_CLEAR(UCSR1A,U2X1);
			UCSR1C = (3<<UCSZ10);
	}

	DISABLE_RX_ISR();

	//reset indexs
	RxRead=0;
	RxWrite=0;
	TxRead=0;
	TxWrite=0;

	Baud=(FCLK_IO/(UINT32)ISO_PRESCALE)/10400;
		
	return 0;
}


/******************************************************************
 ** ISR
 *
 *  DESCRIPTION:
 *
 *
 *  Create: 7/23/2006	5:12:28 PM - Trampas Stern
 *******************************************************************/
ISR(TIMER2_OVF_vect)
{
	static UINT16 ticks=0;
	static UINT8 Isr_count=0;
	static UINT8 txState=UNKNOWN;
	static UINT8 Bits;
	static UINT8 prev=0; 
	static UINT8 IsrData;
	
	ticks++; 
	if (ticks==baudTicks)
	{
		//This section gets call twice as fast as the bit rate
		// It handles the transmission of the data 
		Isr_count++;
		switch(txState)
		{
			case COLLISION:
			case UNKNOWN:	//At this point we need to wait for bus to go idle
				TX_PIN(1);
				ENABLE_RX_ISR();
				if (RX_PIN==0)
				{
					Isr_count=0; 
				}
				if (Isr_count>=8) //idle is a high bit for more than 1 byte time
				{
					txState=IDLE;
				}
				break;
		   	case IDLE:	  //bus is idle 
				Isr_count=0;
			   	if (RX_PIN==0)	//if bus is low, we are not idle
			   	{
				   txState=UNKNOWN;
	 			}else 
				{   // YES we are idle lets see if we have data to transmit
					if (TxRead==TxWrite)
					{	//No data to transmit, lets go away
						DISABLE_OVF_ISR();
						ENABLE_RX_ISR();
						txState=UNKNOWN;
			   		}else
			   		{	//Lets transmit our data
						DISABLE_RX_ISR();
						IsrData=TxBuffer[TxRead];
						txState=TX_START;
					}
				}				
				break;
  			case TX_START:	 //Send out a start bit
				Bits=0x01;	 //init our bit counter
				prev=0;
			   	TX_PIN(0);	 //drive the bus low
			   	if (Isr_count>1)
			   	{	
					Isr_count=0;
					txState=TX_BITS;  //after two calls go to transmitting bits
				}
				break;
			case TX_BITS:
				if (RX_PIN!=prev)	//check that our rx_bit matchs tx_bit				
				{	//If no match then lets give up
					TX_PIN(1);	  //drive bus to idle state before giving up
					Isr_count=0;
					txState=COLLISION; 
					break;
				}
				if (Bits & IsrData)	   //set previous bit state
					prev=1;
				else
					prev=0;
				TX_PIN(prev);	   //transmit bit on bus
				if (Isr_count>1)
			   	{
					Isr_count=0;
					Bits=Bits<<1;
					if (Bits==0)
					{
						txState=TX_STOP;
					}
					if (RX_PIN!=prev)  //check that bit is transmitted correct
					{
						TX_PIN(1);
						txState=COLLISION; //If not give up and we will try again
					}
				}		
				break;
			case TX_STOP:
				TX_PIN(1);
				if (Isr_count>1)
				{
					//we have transmitted data so increment pointer
					TxRead++;
					if (TxRead>=ISO_BUFFER_SIZE)
						TxRead=0;
					//read UDR to clear echo
					//while((UCSRA & 0x80)==0);
					//temp=UDR; 
					//ENABLE_RX_ISR();
					txState=IDLE;
					
				}
				break;
			default:
				txState=UNKNOWN;
				break;
		} //switch(txState)

		ticks=0;
	}
 } //ISR 


//lets attach header and CRC and send a message
//we will return length of response
UBYTE iso_send(UBYTE *resp, UBYTE len, UBYTE *msg, UBYTE msg_len, UBYTE address)
{
	UBYTE keys[2];
	UBYTE ret,i;
	UBYTE message[20];

	//printf_P(PSTR("Connect timer %lu\n\r",connect_timer(0));
	if (connect_timer(0)>300)
		iso_state=ISO_UNKNOWN;
	if (msg_len>6)
	{
		printf_P(PSTR("Error: ISO message allows only 7 bytes\n\r"));
		return 0;
	}
	//if we are not connected then lets connect to the ECM
	if (iso_state==ISO_UNKNOWN)
	{
		//first call the fast connect
		//oops not done yet
		//then call the 5baud connect
		ret=iso_5baud(keys,2,address);

		if (ret>=2)
		{
			//we are connected
			iso_state=ISO_CONNECT;
			//connect_timer(1); //reset timer
		}else
		{
			printf_P(PSTR("Error: ISO slow connect failed\n\r"));
			return 0;
		}
	}
	//OK we need to build the message
	message[0]=0x68;
	message[1]=0x6A;
	message[2]=0xF1;

	for(i=0; i<msg_len; i++)
	{
		message[3+i]=msg[i];
	}

	i=3+i;
	message[i]=checksum(message,i);
	i++;

	//now we can send message
	ret=iso_put(message,i,ISO_P3_MIN);
	if(ret!=i)
	{
		printf_P(PSTR("Error: ISO message sending problems\n\r"));
		iso_state= ISO_UNKNOWN;
		return 0;
	}
	//now lets get response
	ret=iso_get(resp,len,ISO_P3_MAX);

	if (ret)
		connect_timer(1); //reset connection timer

	return ret;
}
	

UBYTE iso_get(UBYTE *data, UBYTE len, UWORD time_out_ms)
{
  	return iso_getAuto(data,len,time_out_ms,0);
}
 


//Well I need an Async get char function
UBYTE iso_getAuto(UBYTE *data, UBYTE len, UWORD time_out_ms,UBYTE autoBaud )
{

	UBYTE temp;	
	UBYTE index;
	TIME t;

 	index=0;

	K_OUT(1);	   //make sure bus is high... 

	//setup timer control
	TCCR3A=0x00;  //disable all compare
	TCCR3B=0x02; //clkio/8

	//UCSR1B =  _BV(RXEN1);
	BIT_CLEAR(UCSR1B,RXEN1);

	
	while(index<len)
	{
		GetTime(&t);
		while(K_IN==0)
		{
			if ((UINT16)GetElaspMs(&t)>ISO_P2_MAX )
			{
				printf_P(PSTR("Error: ISO get bus error\n\r"));
				return index;
			}
		}
		
		BIT_SET(UCSR1B,RXEN1);

		
/*		
		GetTime(&t);

		while(K_IN==1)
		{
			if ((UINT16)GetElaspMs(&t)>time_out_ms )
			{
				if (index==0)
				{
					printf_P(PSTR("Error: ISO get time out\n\r");
				}
				return index;
			}
		}
		TCNT3=0;		
		//if baud is zero assume user wants auto baud

		while(K_IN==0);	//time start bit
		if (autoBaud)
		{
			Baud=TCNT3;
			autoBaud=0;
		}
	
		TCNT3=0;	
		while(TCNT3<(Baud/2));
	
		//now clock in bits	
		temp=0;
		for(i=0; i<8; i++)
		{
			temp=temp>>1;
			if (K_IN)
			{
				temp=temp | 0x80;
			}
			TCNT3=0;		
			while(TCNT3<Baud);
		}	   
		*data=temp;
*/
		GetTime(&t);
		while (BIT_TEST(UCSR1A,RXC1)==0)
		{
			if ((UINT16)GetElaspMs(&t)>(time_out_ms*4) )
			{
				if (index==0)
				{
					printf_P(PSTR("Error: ISO get time out\n\r"));
				}
				BIT_CLEAR(UCSR1B,RXEN1);
				return index;
			}
		}
			
		temp=UDR1;
		//BIT_CLEAR(UCSR1B,RXEN1);
		*data=temp;
		data++;
		index++;
		time_out_ms=ISO_W2_MAX;
	}
	BIT_CLEAR(UCSR1B,RXEN1);
	return index;
}

/*
UDWORD iso_Baud()
{
   float temp;

   temp=1000000.0/((float)baud*0.8);
   return (UDWORD)temp;
}
*/
		

#if 0
//well life is a bitch then you die... 
//I need some way of testing ISO interface
//So I need a monitor
#define ISO_MONITOR_IN K_IN
UBYTE iso_monitor()
{
	UWORD ms_timer;
	UBYTE count,i;
	UBYTE data[20];
	
	printf_P(PSTR("Monitor started\n\r")); 
	printf_P(PSTR("Hit any key to stop\n\r"));

	//lets wait for some activity on the 
	count=0;
	ms_timer=0;
	TMR0=0;
	while (ISO_MONITOR_IN==1 && ms_timer<=3000)
	{
	   if (TMR0>=250)
		{
			TMR0=0;
			count++;
			if (count>=5)
			{
				ms_timer++;
				count=0;
			}
		}
	}
	if (ms_timer>=3000)
	{
		printf_P(PSTR("high for over 3sec\n\r"));
		return 0;
	}
	disable_interrupts(GLOBAL);
	TMR0=0;
	count=0;
	ms_timer=0;
	while (ISO_MONITOR_IN==0 && ms_timer<3000)
	{
	   if (TMR0>=250)
		{
			TMR0=0;
			count++;
			if (count>=5)
			{
				ms_timer++;
				count=0;
			}
		}
	}
	count=TMR0;
	TMR0=0;

	if (ms_timer>=3000)
	{
		printf_P(PSTR("low for over 3sec\n\r"));
		enable_interrupts(GLOBAL);
		return 0;
	}

	
	//we have just got a low pulse
	if (ms_timer>=23 && ms_timer<=28)
	{
		UBYTE i;
		//we have fast init
		//printf_P(PSTR("Fast\n\r");

		//now lets wait process data
		//data=iso_getc();
		disable_interrupts(GLOBAL);
		count=iso_get(data,20,3000);
		enable_interrupts(GLOBAL);
	  	printf_P(PSTR("\n\r\n\rFast Init %u "),count);
		for(i=0; i<count; i++)
		{
			fprintf(STDIN,"%X ", data[i]);
		}
	}else
	if (ms_timer>=175 && ms_timer<225)
	{
		//we have slow init
		//printf_P(PSTR("Slow\n\r");
		delay_ms(1600);	 //wait for rest of slow bits
		disable_interrupts(GLOBAL);
		count=iso_get(data,20,3000);
		enable_interrupts(GLOBAL);
		if (count==0)
		{
			printf_P(PSTR("No response from ECM Slow\n\r"));
			//return 0;
		}
		printf_P(PSTR("\n\r\n\rSlow Init %u "),count);
		for(i=0; i<count; i++)
		{
			fprintf(STDIN,"%X ", data[i]);
		}

	}else 
	if (ms_timer<23 || ms_timer>225)
	{
		printf_P(PSTR("Unknown ms=%lu,TMR0=%u\n\r"),ms_timer,count);
		enable_interrupts(GLOBAL);
		return 0;
	}

	//lets recieve data
	while(!kbhit())
	{
	  	disable_interrupts(GLOBAL);
	  	count=iso_get(data,20,3000);
		enable_interrupts(GLOBAL);
	  	printf_P(PSTR("\n\rFast %u ",count);
		for(i=0; i<count; i++)
		{
			fprintf(STDIN,"%X ", data[i]);
		}
	}
	return 0;
	
}

#endif


UBYTE iso_fast_init(UWORD idle_ms)
{
	TIME t;
	GetTime(&t);


	//first part of the initilization process is to
	//wait for bus to be idle
	while(GetElaspMs(&t)<idle_ms)
	{
		if (K_IN==0)
			return 0; //bus in use

	}

	//ok bus is idle, so lets send a init pulse
	K_OUT(0);
	L_OUT(0);
	delay_ms(25);
	K_OUT(1);
	L_OUT(1);
	//delay for rest of Twup

	//now send some data
	return 0;
}

//sends out a char at 5-baud rate
UWORD iso_5baud_putc(UBYTE data)
{ 
	UBYTE i;

	BIT_CLEAR(UCSR1C,TXEN1);
	//first drive low for start bit
	K_OUT(0);
	L_OUT(0);
	delay_ms(200);
	for(i=0; i<8; i++)
	{
		if (data & 0x01)
		{
			K_OUT(1);
			L_OUT(1);
		} else
		{
			K_OUT(0);
			L_OUT(0);
		}
		data=data>>1;
		delay_ms(200);
	}
	//output stop bit
	K_OUT(1);
	L_OUT(1);
	delay_ms(200);

	return 0;
}
	
		  
	
		

//preform 5 baud initlization
//returns the time for the baud rate
UWORD iso_5baud(UBYTE *keys, UBYTE len, UBYTE address)
{
	UBYTE data;
	UBYTE i,temp;
	TIME t;
	

	//first make sure that the buss is high for at least W0



	while(GetElaspMs(&t)<=ISO_W0_MIN)
	{
		if (K_IN==0)
			return 0;
	}
	printf_P(PSTR("5 baud addr=%x\n\r"),address);
	iso_5baud_putc(address);

	data=0;
	temp=iso_getAuto(&data,1,ISO_W1_MAX*2,1); //auto baud
	//printf_P(PSTR("got %d %x\n\r",temp,data);
	if (data!=0x55)
	{
		printf_P(PSTR("Bad 5baud init %u %X\n\r"),(int)temp,(int)data);  
		return 0;
	}
	//baud=ISO_10400_BAUD;

	//TODO handle none irregular baud rates...
	data=iso_get(keys,len,ISO_W2_MAX);
	if (data>=2)
	{
		temp=~keys[1];
		//fputc(temp,K_FAST);
		iso_put(&temp,1,ISO_W4_MIN);  
		if (address==0x33)
		{
			temp=0;
			i=iso_get(&temp,1,ISO_W4_MAX*2);
			if (i==0 ||  temp!=0xCC)
			{
				printf_P(PSTR("Error: ISO 5 baud failed 0x%X 0x%X %d\n\r"),keys[1],temp,i);
				return 0; 
			}
		}
	} else
	{
		printf_P(PSTR("Bad keys\n\r"));
	}	 
	iso_state=ISO_CONNECT;
	return data;
}

	

UBYTE iso_put(UBYTE *data, UBYTE len, UWORD idle_wait)
{
	UBYTE bits;
	UBYTE temp;
	UBYTE index;
	TIME t;
		


	//setup timer control
	TCCR3A=0x00;  //disable all compare
	TCCR3B=0x02; //clkio/8

	//printf_P(PSTR("put baud =%d\n",baud);

	//make sure buss is high
	//TCNT3=0;
	GetTime(&t);

	while(K_IN==0);		  
			  
	while((UINT16)GetElaspMs(&t)<idle_wait)
	{
		if (!K_IN)
		{
			//GetTime(&t);
			printf_P(PSTR("Error: Put failed\n\r"));
			return 0;
		}
   	}
	BIT_CLEAR(UCSR1B,TXEN1);
	//BIT_SET(UCSR1B,TXEN1);

	//printf_P(PSTR("ms timer %lu\n\r",timer_ms);

	//for each bit.... 
    index=0;
	while(index<len)
	{
		UBYTE mask;


		temp=data[index];
		
		//while(BIT_TEST(UCSR1A,UDRE1)==0);
		//UDR1=temp;

		mask=0x01;
		//drive with start bit, then send data
		K_OUT(0);
   		TCNT3=0;
		while(TCNT3<Baud);
		TCNT3=0;
		for (bits=0; bits<8; bits++)
		{
			if (temp & mask)
			{
				K_OUT(1);
			}else
			{
				K_OUT(0);
			}		
			mask=mask<<1;
			while(TCNT3<Baud);
			TCNT3=0;
		}

		//now we need to send stop bit
		K_OUT(1);
		TCNT3=0;
		while(TCNT3<Baud);

		index++;
		if(index>=len)
		{
			break;
		}
		
		//now wait to send next byte
		GetTime(&t);
		while((UINT16)GetElaspMs(&t)<ISO_P4_MIN)
		{
			if (!K_IN)
			{
				printf_P(PSTR("ERROR: ISO put failed\n\r"));
				//while(BIT_TEST(UCSR1A,TXC1)==0);
				//BIT_CLEAR(UCSR1B,TXEN1);
				return 0;
			}

		}

	}
	//while(BIT_TEST(UCSR1A,UDRE1)==0); //wait for all data to be transmited
	return index;
}
		

	
		


