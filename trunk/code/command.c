/*
Copyright (C) Trampas Stern  name of author

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "command.h"
#include <string.h>
#include "serial.h"
#include "codes.h"
#include "flash.h"

UINT CommandParse(CHAR *str);
UINT 

#define COMMAND(NAME)  { NAME ## _str, NAME ## _cmd, NAME ## _help}
#define PGM_STR(NAME,STR) const char NAME ## _help[] PROGMEM = STR;  const char NAME ## _str[] PROGMEM = #NAME; 


//Command structure
struct Command
{
	PGM_P name;
	int (*function) (int, char **);
	PGM_P help;
};



/************************************************
When creating a new command there are  three things that
need to be done.
1) make function and prototype 
2) create the help string using the PGM_STR macro
3) add the command to the commands[] structure 
    using the COMMAND macro
*************************************************/

//Prototypes
int help_cmd(int, char **);
int exit_cmd(int, char **);
int flashp_cmd(int, char **);
int flashb_cmd(int, char **);
int flashu_cmd(int, char **);
int flashc_cmd(int, char **);




//Help String defintions 
PGM_STR(help,"Displays this message");
PGM_STR(exit,"Exits command mode");
PGM_STR(flashp,"Xmodem P-codes to flash"); 
PGM_STR(flashu,"Xmodem U-codes to flash");
PGM_STR(flashb,"Xmodem B-codes to flash");
PGM_STR(flashc,"Xmodem C-codes to flash");


//List of supported commands
struct Command commands[] PROGMEM  =
{
	COMMAND(help),
	COMMAND(exit),
	COMMAND(flashp),
	COMMAND(flashu),
	COMMAND(flashb),
	COMMAND(flashc),
	{"",0,""}, //End of list signal
};



UINT CommandExit=0;


int flashp_cmd(int argc, char * argv[])
{
	printf_P(PSTR("Start Xmodem of P Codes\n"));
	return Flash_serial_program(PCODE_FLASH_START);
}

int flashu_cmd(int argc, char * argv[])
{
	printf_P(PSTR("Start Xmodem of U Codes\n"));
	return Flash_serial_program(UCODE_FLASH_START);
}

int flashb_cmd(int argc, char * argv[])
{
	printf_P(PSTR("Start Xmodem of B Codes\n"));
	return Flash_serial_program(BCODE_FLASH_START);
}

int flashc_cmd(int argc, char * argv[])
{
	printf_P(PSTR("Start Xmodem of C Codes\n"));
	return Flash_serial_program(CCODE_FLASH_START);
}


/* print out the help strings for the commands */
int help_cmd(int argc, char * argv[])
{
	struct Command cmd_list; 
	int i;

	//now let's parse the command
	i=0;
	memcpy_P(&cmd_list, &commands[i], sizeof(struct Command));
	while(cmd_list.function!=0)
	{
		
		printf_P(cmd_list.name);
		printf_P(PSTR(" - "));
		printf_P(cmd_list.help);
		printf_P(PSTR("\n\r"));
	   	i=i+1;
		memcpy_P(&cmd_list, &commands[i], sizeof(struct Command));
	}
	return 0;
}

int exit_cmd(int argc, char * argv[])
{
	CommandExit=1;
	return 0;
}



UBYTE ascii2hex(UBYTE *str)
{
 	UBYTE j;
	UBYTE temp,t2,shift;
	temp=0;
	for (j=0; j<2; j++)
	{
		t2=str[j];
		shift=0; 

		if (j==0)
			shift=4;

		if (t2>='a')
		{
			temp=temp | ((t2-'a'+10)<<shift);
		}else
		if (t2>='A')
		{
			temp=temp | ((t2-'A'+10)<<shift);
		}else
		if (t2>='0')
		{
			temp=temp | ((t2-'0')<<shift);
		}
	}
	return temp;
}	
/*******************************************************************
 *  FUNCTION: CommandBegin
 *  AUTHOR 		= 	TRAMPAS STERN
 *  FILE 		=	command.c	
 *  DATE		=   1/25/2004  3:43:22 PM
 *
 *  PARAMETERS: 	
 *
 *  DESCRIPTION: Starts up the serial command line 
 *
 *  RETURNS: 
 *
 *
 *******************************************************************/
UINT CommandBegin()
{
	UBYTE data;
	CHAR buffer[MAX_STRING];
	UBYTE buffIndex;

	//first clear the screen and push command prompt
	CLEAR_SCREEN();
	printf_P(PSTR(CMD_PROMPT));

	//now spin reading command and waiting on CR
	CommandExit=0;
	buffIndex=0;
	buffer[0]=0; 
	while(!CommandExit)
	{
		if(uart_kbhit())
		{
			data=uart_getchar();
				
			//echo the data
			uart_putchar(data);
		

			//if the data is the CR we need to process buffer
			if (data==0x0D)
			{	
				uart_putchar(0x0A);
				if (strlen(buffer)>0)
					CommandParse(buffer);
				printf_P(PSTR("\n\r"));
				printf_P(PSTR(CMD_PROMPT));
				buffIndex=0;
				buffer[buffIndex]=0;
			} 
		


			if (data != 0x0A && data !=0x0D)
			{
				buffer[buffIndex++]=data;
				buffer[buffIndex]=0;
			}
			if (buffIndex>=(MAX_STRING-1))
			{
				printf("\n\rERROR: Command buffer overflow\n\r");
				buffIndex=0;
				buffer[0]=0;
				printf(CMD_PROMPT);
			}
		}
	}
	return 0;
}

/*******************************************************************
 *  FUNCTION: CommandParse
 *  AUTHOR 		= 	TRAMPAS STERN
 *  FILE 		=	command.c	
 *  DATE		=   1/25/2004  4:03:03 PM
 *
 *  PARAMETERS: Takes the command line string	
 *
 *  DESCRIPTION: parses the command line and returns Command ID
 *
 *  RETURNS: Command ID, and 
 *
 *
 *******************************************************************/
UINT CommandParse(CHAR *str)
{
	CHAR *ptr;
	CHAR *ptr2;
	UINT i;
	CHAR cmd[MAX_STRING];
	CHAR buff[MAX_CMD_LENGTH];
	CHAR argv[MAX_ARGS][MAX_ARG_LENGTH];
	CHAR *ptrArgv[MAX_ARGS];
	UINT numArgs;

	struct Command cmd_list; 

	//first we need find command and arguments	
	ptr=strchr(str,' '); //find first char

	if (ptr==0)
	{
		//we have two options, frist whole thing is command
		//second bad command
		if(strlen(str)>0)
			ptr=str+strlen(str); 
		else
			return 0; //bad command
	}

	//copy string to command buffer. 
	i=0;
	ptr2=str; 
	while(ptr!=0 && ptr!=ptr2 && i<(MAX_CMD_LENGTH-1))
	{
		buff[i++]=*ptr2++; 
	}
	buff[i]=0;

	//now buff contains the command let's get the args
	numArgs=0;
	while(*ptr!=0 && *ptr==' ')
		ptr++; //increment pointer past ' '
	if (*ptr!=0)
	{ 	
		ptr2=strchr(ptr,' ');
		if (ptr2==0)
		{
			//we have two options, frist whole thing is command
			//second bad command
			if(strlen(ptr)>0)
				ptr2=ptr+strlen(ptr); 
		}
		while(ptr2!=0 && numArgs<MAX_ARGS)
		{
			int j;
			j=0;
			while (ptr2!=ptr && j<(MAX_ARG_LENGTH-1))
			{
				argv[numArgs][j++]=*ptr++;
			}
			argv[numArgs][j++]=0;
			numArgs++;
			ptr2=0;
			if (*ptr!=0)
			{
				while(*ptr!=0 && *ptr==' ')
					ptr++; //increment pointer past ' '
				ptr2=strchr(ptr,' ');
				if (ptr2==0)
				{
					//we have two options, frist whole thing is command
					//second bad command
					if(strlen(ptr)>0)
						ptr2=ptr+strlen(ptr); 
				}
			}
		}
	}

	for(i=0; i<MAX_ARGS; i++)
	{
		ptrArgv[i]=argv[i];
	}
	
	//now let's parse the command
	i=0;
	memcpy_P(&cmd_list, &commands[i], sizeof(struct Command));
	while(cmd_list.function!=0)
	{
		
		//memcpy_P(&p, cmd_list.name, sizeof(PGM_P));
		if (strcmp_P(buff,cmd_list.name)==0)
		{
			//printf(buff);
			//return 1;
			return (*cmd_list.function)(numArgs,ptrArgv);
		}
		i=i+1;
		memcpy_P(&cmd_list, &commands[i], sizeof(struct Command));
	}
	return 0;
}
