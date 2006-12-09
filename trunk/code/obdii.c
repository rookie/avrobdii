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

#include "obdii.h"
#include "codes.h"
#include "system.h"
#include "leds.h"
#include "time.h"
#include "iso.h"
#include "lcd.h"

UBYTE connect=UNKNOWN; 
UBYTE obdError=0;
UBYTE pidSupported[32]={0}; //allow up to 256 PIDS
UBYTE pid13;
UBYTE pid1D;
UBYTE MIL_ON=0;

UBYTE obdII_clear_codes()
{
	OBDII data[MAX_RESP];
	UBYTE cmd,ret,i;
	
	cmd=0x04;
	ret=obdII_send(&cmd,1,data,MAX_RESP);
	
	if (ret==0)
	{
		printf_P(PSTR("No response to clear codes\n\r"));
		return 0;
	}
	for (i=0; i<ret; i++)
	{
		if (data[i].data[0]==0x44)
		{
			printf_P(PSTR("ECM %X codes cleared\n\r"),data[i].addr);
		}
	}
	return ret;
}


UBYTE pidLabels(UBYTE pid, UBYTE *str, UBYTE nStr, UBYTE *str2, UBYTE nStr2)
{
 	str[0]=0;
	str2[0]=0;
 	switch (pid)
	{
		case 0x03:
			sprintf_P(str,PSTR("Fuel Sys1: "));
			sprintf_P(str2,PSTR("Fuel Sys2: "));
			break;					
		case 0x04:
			sprintf_P(str,PSTR("Load "));
			break;
		case 0x05: 
			sprintf_P(str,PSTR("ECT "));
			break;
		case 0x06: 
			sprintf_P(str,PSTR("SHRTFT1: "));
			if (pid1D & 0x30)
			{
				sprintf_P(str2,PSTR("SHRTFT3: "));
			}
			break;
		case 0x07: 
			sprintf_P(str,PSTR("LONGFT1: "));
			if (pid1D & 0x30)
			{
				sprintf_P(str2,PSTR("LONGFT3: "));
			}
			break;
		case 0x08: 
			sprintf_P(str,PSTR("SHRTFT2: "));
			if (pid1D & 0xC0)
			{
				sprintf_P(str2,PSTR("SHRTFT4: "));
			}
			break;
		case 0x09: 
			sprintf_P(str,PSTR("LONGFT2: "));
			if (pid1D & 0xC0)
			{
				sprintf_P(str2,PSTR("LONGFT4: "));
			}
			break;
		case 0x0A: 
			sprintf_P(str,PSTR("Fuel P: "));
			break;
		case 0x0B: 
			sprintf_P(str,PSTR("MAP: "));
			break;
		case 0x0C: 
			sprintf_P(str,PSTR("RPM: "));
			break;
		case 0x0D: 
			sprintf_P(str,PSTR("VSS: "));
			break;
		case 0x0E: 
			sprintf_P(str,PSTR("Spark Adv: "));
			break;
		case 0x0F: 
			sprintf_P(str,PSTR("IAT: "));
			break;
		case 0x10: 
			sprintf_P(str,PSTR("MAF: "));
			break;
		case 0x11: 
			sprintf_P(str,PSTR("TPS: "));
			break;
		case 0x12: 
   			sprintf_P(str,PSTR("Air Stat: "));
			break;
		case 0x14: 
			sprintf_P(str,PSTR("O2 B1S1: "));
			sprintf_P(str2,PSTR("O2 B1S1: "));
			break;
		case 0x15: 
			sprintf_P(str,PSTR("O2 B1S2: "));
			sprintf_P(str2,PSTR("O2 B1S2: "));
			break;
		case 0x16: 
			sprintf_P(str,PSTR("O2 B1S3: "));
			sprintf_P(str2,PSTR("O2 B1S3: "));
			break;
		case 0x17: 
			sprintf_P(str,PSTR("O2 B1S4: "));
			sprintf_P(str2,PSTR("O2 B1S4: "));
			break;
		case 0x18: 
			sprintf_P(str,PSTR("O2 B2S1: "));
			sprintf_P(str2,PSTR("O2 B2S1: "));
			break;
		case 0x19: 
			sprintf_P(str,PSTR("O2 B2S2: "));
			sprintf_P(str2,PSTR("O2 B2S2: "));
			break;
		case 0x1A: 
			sprintf_P(str,PSTR("O2 B2S3: "));
			sprintf_P(str2,PSTR("O2 B2S3: "));
			break;
		case 0x1B: 
			sprintf_P(str,PSTR("O2 B2S4: "));
			sprintf_P(str2,PSTR("O2 B2S4: "));
			break;
		case 0x1C: 
			sprintf_P(str,PSTR("OBDSUP: "));
			break;
		case 0x1E: 
			sprintf_P(str,PSTR("PTO_STAT: "));
			break;
		case 0x1F: 
			sprintf_P(str,PSTR("RunTime: "));
			break;
		case 0x21: 
			sprintf_P(str,PSTR("MIL_dist: "));
			break;
		case 0x22: 
			sprintf_P(str,PSTR("FRP: "));
			break;
		case 0x23: 
			sprintf_P(str,PSTR("FRP: "));
			break;
		case 0x24:
		case 0x34: 
			sprintf_P(str,PSTR("EQ_RAT11: "));
			sprintf_P(str2,PSTR("O2 S11: "));
   			break;
		case 0x25: 
		case 0x35:
			sprintf_P(str,PSTR("EQ_RAT12: "));
			sprintf_P(str2,PSTR("O2 S12: "));
   			break;	
   		case 0x26:
   		case 0x36: 
			sprintf_P(str,PSTR("EQ_RAT13: "));
			sprintf_P(str2,PSTR("O2 S13: "));
   			break;
		case 0x27: 
		case 0x37:
			sprintf_P(str,PSTR("EQ_RAT14: "));
			sprintf_P(str2,PSTR("O2 S14: "));
   			break;
		case 0x28: 
		case 0x38:
			sprintf_P(str,PSTR("EQ_RAT21: "));
			sprintf_P(str2,PSTR("O2 S21: "));
   			break;
		case 0x29:
		case 0x39:
			sprintf_P(str,PSTR("EQ_RAT22: "));
			sprintf_P(str2,PSTR("O2 S22: "));
   			break;
		case 0x2A: 
		case 0x3A:
			sprintf_P(str,PSTR("EQ_RAT23: "));
			sprintf_P(str2,PSTR("O2 S23: "));
   			break;
		case 0x2B: 
		case 0x3B:
			sprintf_P(str,PSTR("EQ_RAT24: "));
			sprintf_P(str2,PSTR("O2 S24: "));
   			break;
   		case 0x2C: 
			sprintf_P(str,PSTR("EGR: "));
			break;
		case 0x2D: 
			sprintf_P(str,PSTR("EGR_ERR: "));
			break;
		case 0x2E: 
			sprintf_P(str,PSTR("EVAP: "));
			break;
		case 0x2F: 
			sprintf_P(str,PSTR("Fuel Lvl: "));
			break;
		case 0x30: 
			sprintf_P(str,PSTR("Warm ups: "));
			break;
		case 0x31: 
			sprintf_P(str,PSTR("Clear Dist: "));
			break;
		case 0x32: 
			sprintf_P(str,PSTR("EVAP P: "));
			break;
		case 0x33: 
			sprintf_P(str,PSTR("BARO: "));
			break;
		case 0x3C: 
			sprintf_P(str,PSTR("CAT TEMP11: "));
			break;
		case 0x3D: 
			sprintf_P(str,PSTR("CAT TEMP21: "));
			break;
		case 0x3E: 
			sprintf_P(str,PSTR("CAT TEMP12: "));
			break;
		case 0x3F: 
			sprintf_P(str,PSTR("CAT TEMP22: "));
			break;
		case 0x42:
			sprintf_P(str,PSTR("VPWR: "));
			break;
		case 0x43:
			sprintf_P(str,PSTR("LOAD ABS: "));
			break;
		case 0x44:
			sprintf_P(str,PSTR("EQ_RAT: "));
			break;
		case 0x45:
			sprintf_P(str,PSTR("TP_R: "));
			break;
		case 0x46:
			sprintf_P(str,PSTR("AAT: "));
			break;
		case 0x47:
			sprintf_P(str,PSTR("TPS_B: "));
			break;
		case 0x48:
			sprintf_P(str,PSTR("TPS_C: "));
			break;
		case 0x49:
			sprintf_P(str,PSTR("APP_D: "));
			break;
		case 0x4A:
			sprintf_P(str,PSTR("APP_E: "));
			break;
		case 0x4B:
			sprintf_P(str,PSTR("APP_F: "));
			break;
		case 0x4C:
			sprintf_P(str,PSTR("TAC_PCT: "));
			break;
		case 0x4D:
			sprintf_P(str,PSTR("MIL_TIME: "));
			break;
		case 0x4E:
			sprintf_P(str,PSTR("CLR_TIME: "));
			break;
	}
	return 0;
}


//we have a OBDII data stream which should be a PID
UBYTE process_PID(UBYTE *data, UBYTE *str, UBYTE nStr, UBYTE *str2, UBYTE nStr2)
{
	UBYTE pid,ret;
	
	str[0]=0;
	str2[0]=0;

	if (data[0]!=0x41)
		return 0;	  //not a PID 
			
	pid=data[1]; //get PID
	ret=1;
	switch (pid)
	{
		case 0x03:
			if (data[2]==0)
				sprintf_P(str,PSTR("N/A"));
			if (data[2] & 0x01)
				sprintf_P(str,PSTR("OL"));
			if (data[2] & 0x02)
				sprintf_P(str,PSTR("CL"));
			if (data[2] & 0x04)
				sprintf_P(str,PSTR("OL-Dr"));
			if (data[2] & 0x08)
				sprintf_P(str,PSTR("OL_Flt"));
			if (data[2] & 0x10)
				sprintf_P(str,PSTR("CL_Flt"));
			if (data[3]==0)
				sprintf_P(str2,PSTR("N/A"));
			if (data[3] & 0x01)
				sprintf_P(str2,PSTR("OL"));
			if (data[3] & 0x02)
				sprintf_P(str2,PSTR("CL"));
			if (data[3] & 0x04)
				sprintf_P(str2,PSTR("OL-Dr"));
			if (data[3] & 0x08)
				sprintf_P(str2,PSTR("OL_Flt"));
			if (data[3] & 0x10)
				sprintf_P(str2,PSTR("CL_Flt"));

			break;					
		case 0x04:
			sprintf_P(str,PSTR("%01.1f%%"),OBDII_PERCENT(data[2]));
			break;
		case 0x05: 
			sprintf_P(str,PSTR("%01.0f%cC"),OBDII_TEMP(data[2]),0xDF);
			break;
		case 0x06: 
		case 0x07:
		case 0x08:
		case 0x09:
			sprintf_P(str,PSTR("%01.0f%%"),OBDII_O2PERCENT(data[2]));
			sprintf_P(str2,PSTR("%01.0f%%"),OBDII_O2PERCENT(data[3]));
			break;
		case 0x0A: 
			sprintf_P(str,PSTR("%01.0fkPa"),(double)(data[2])*3.0);
			break;
		case 0x0B: 
			sprintf_P(str,PSTR("%01.0fkPa"),(double)(data[2]));
			break;
		case 0x0C: 
			sprintf_P(str,PSTR("%01.0f"),(double)MAKEWORD(data[2],data[3])/4.0);
			break;
		case 0x0D: 
			sprintf_P(str,PSTR("%01.0fkm/h"),(double)(data[2]));
			break;
		case 0x0E: 
			sprintf_P(str,PSTR("%01.0f"),((double)(data[2])-128.0)/2.0);
			break;
		case 0x0F: 
			sprintf_P(str,PSTR("%01.0f%cC"),OBDII_TEMP(data[2]),0xDF);
			break;
		case 0x10: 
			sprintf_P(str,PSTR("%01.2fg/s"),(double)MAKEWORD(data[2],data[3])*0.01);
			break;
		case 0x11: 
			sprintf_P(str,PSTR("%01.2f%%"),OBDII_PERCENT(data[2]));
			break;
		case 0x12: 
			if (data[2] & 0x01)
				 sprintf_P(str,PSTR("UPS"));
			if (data[2] & 0x02)
				 sprintf_P(str,PSTR("DNS"));
			if (data[2] & 0x04)
				 sprintf_P(str,PSTR("OFF"));
			break;
		case 0x14: 
		case 0x15: 
		case 0x16: 
		case 0x17: 
		case 0x18: 
		case 0x19: 
		case 0x1A:
		case 0x1B:  
			sprintf_P(str,PSTR("%01.3fV"),OBDII_O2VOLTS(data[2]));
			sprintf_P(str2,PSTR("%01.1f%%"),OBDII_O2PERCENT(data[3]));
			break;
		case 0x1C:
			if(data[2]==0x01)
				sprintf_P(str,PSTR("OBDII"));
			if(data[2]==0x02)
				sprintf_P(str,PSTR("OBD"));
			if(data[2]==0x03)
				sprintf_P(str,PSTR("OBD & II"));
			if(data[2]==0x04)
				sprintf_P(str,PSTR("OBD I"));
			if(data[2]==0x05)
				sprintf_P(str,PSTR("No OBD"));
			if(data[2]==0x06)
				sprintf_P(str,PSTR("EOBD"));
			if(data[2]==0x07)
				sprintf_P(str,PSTR("EOBD&II"));
			if(data[2]==0x08)
				sprintf_P(str,PSTR("EOBD&OBD"));
			if(data[2]==0x09)
				sprintf_P(str,PSTR("E OBD II"));
			if(data[2]==0x0A)
				sprintf_P(str,PSTR("JOBD"));
			if(data[2]==0x0B)
				sprintf_P(str,PSTR("JOBD II"));
			if(data[2]==0x0C)
				sprintf_P(str,PSTR("JOBD EOBD"));
			if(data[2]==0x0D)
				sprintf_P(str,PSTR("J E II"));
			break;
		case 0x1F: 
			sprintf_P(str,PSTR("%0us"), MAKEWORD(data[2],data[3]) );
			break;
		case 0x21: 
			sprintf_P(str,PSTR("%0ukm"), MAKEWORD(data[2],data[3]));
			break;
		case 0x22: 
			sprintf_P(str,PSTR("%01.2fkPA"),(double)MAKEWORD(data[2],data[3])*0.079 );
			break;
		case 0x23: 
			sprintf_P(str,PSTR("%01.0fkPA"),(double)MAKEWORD(data[2],data[3])*10.0);
			break;
		case 0x24:
		case 0x25:
		case 0x26:
		case 0x27:
		case 0x28:
		case 0x29:
		case 0x2A:
		case 0x2B: 
			sprintf_P(str,PSTR("%01.3f"),(double)MAKEWORD(data[2],data[3])*0.0000305);
			sprintf_P(str2,PSTR("%01.2fV"),(double)MAKEWORD(data[2],data[3])*0.000122);
			break;
		case 0x2C: 
			sprintf_P(str,PSTR("%01.1f%%"),OBDII_PERCENT(data[2]));
			break;
		case 0x2D: 
			sprintf_P(str,PSTR("%01.1f%%"),OBDII_O2PERCENT(data[2]));
			break;
		case 0x2E: 
			sprintf_P(str,PSTR("%01.1f%%"),OBDII_PERCENT(data[2]));
			break;
		case 0x2F: 
			sprintf_P(str,PSTR("%01.1f%%"),OBDII_PERCENT(data[2]));
			break;
		case 0x30: 
			sprintf_P(str,PSTR("%0d"),data[2]);
			break;
		case 0x31: 
			sprintf_P(str,PSTR("%0dkm"),MAKEWORD(data[2],data[3]));
			break;
		case 0x32: 
			sprintf_P(str,PSTR("%01.2fPa"),((double)(MAKEWORD(data[2],data[3])))*0.25);
			break;
		case 0x33: 
			sprintf_P(str,PSTR("%0dkPa"),data[2]);
			break;
		case 0x34:
		case 0x35:
		case 0x36:
		case 0x37:
		case 0x38:
		case 0x39:
		case 0x3A:
		case 0x3B: 
			sprintf_P(str,PSTR("%01.3f"),(double)MAKEWORD(data[2],data[3])*0.0000305);
			sprintf_P(str2,PSTR("%01.3fmA"),(double)MAKEWORD(data[2],data[3])*0.00390625);
			break;
		case 0x3C:
		case 0x3D:
		case 0x3E:
		case 0x3F: 
			sprintf_P(str,PSTR("%01.1f%cC"),(double)MAKEWORD(data[2],data[3])*0.1,0xDF);
			break;
		case 0x42:
			sprintf_P(str,PSTR("%01.3fV"),(((double)(MAKEWORD(data[2],data[3])))*0.001)-40.0);
			break;
		case 0x43:
			sprintf_P(str,PSTR("%01.1f%%abs"),((double)(MAKEWORD(data[2],data[3])))*100.0/255.0);
			break;
		case 0x44:
			sprintf_P(str,PSTR("%01.3fs"),((double)(MAKEWORD(data[2],data[3])))*0.0000305);
			break;
		case 0x45:
			sprintf_P(str,PSTR("%01.1f%%"),OBDII_PERCENT(data[2]));
			break;
		case 0x46:
			sprintf_P(str,PSTR("%01f%cC"),OBDII_TEMP(data[2]),0xDF);
			break;
		case 0x47:
			sprintf_P(str,PSTR("%01f%%"),OBDII_PERCENT(data[2]));
			break;
		case 0x48:
			sprintf_P(str,PSTR("%01f%%"),OBDII_PERCENT(data[2]));
			break;
		case 0x49:
			sprintf_P(str,PSTR("%01f%%"),OBDII_PERCENT(data[2]));
			break;
		case 0x4A:
			sprintf_P(str,PSTR("%01f%%"),OBDII_PERCENT(data[2]));
			break;
		case 0x4B:
			sprintf_P(str,PSTR("%01f%%"),OBDII_PERCENT(data[2]));
			break;
		case 0x4C:
			sprintf_P(str,PSTR("%01f%%"),OBDII_PERCENT(data[2]));
			break;
		case 0x4D:
			sprintf_P(str,PSTR("%0ds"),MAKEWORD(data[2],data[3]));
			break;
		case 0x4E:
			sprintf_P(str,PSTR("%0ds"),MAKEWORD(data[2],data[3]));
			break;
	}
	return ret;
}


UBYTE isPidSupported(UBYTE pid)
{
	return BIT_TEST(pidSupported[pid>>3],(pid & 0x07));
}

UBYTE getNextPid(UBYTE pid)
{
	while(pid<0x4E)
	{
		pid++;
		if(isPidSupported(pid) && pid!=0x13 && pid!=0x1d )
			return pid;
	}
	return 0;
}

UBYTE PidInit()
{
	OBDII data[MAX_RESP];	
   	UBYTE cmd[2],ret,i,j;
	UBYTE mask,dataByte;
	UBYTE offset;
	static UBYTE numPids=0;
	static UBYTE ncount=0;
	
	if (ncount==0 || ncount>50)
	{
		numPids=0;
		offset=0;
		ncount=0;
	  	do {
		  	//first find supported PIDs
			cmd[0]=0x01;
			cmd[1]=offset;

			ret=obdII_send(cmd,2,data,MAX_RESP);

			if(ret==0)
			{
				printf_P(PSTR("No response from ECM for PID number\n\r"));
				return 0;
			}
			if (ret>1)
			{
				//or the data bytes together
				printf_P(PSTR("Num ECMs %d\n\r"),ret);
				for(j=0; j<ret; j++)
				{
					for(i=2; i<6; i++)
					{
						data[0].data[i]=data[0].data[i] | data[j].data[i];
					}
				}
			}
/*
			printf_P(PSTR("pids %u: ",data[0].n);
			for(i=0; i<data[0].n;i++)
			{
				printf_P(PSTR("%X ",data[0].data[i]);
			}
			printf_P(PSTR("\n\r"));
*/
			//else lets process PIDs.... 
			i=1+offset;
			mask=0x80;
			dataByte=2;
			while(dataByte<6 && dataByte<data[0].n)
			{
				if (data[0].data[dataByte] & mask)
				{
					BIT_SET(pidSupported[i>>3],(i & 0x07));
					if (i!=0x01)
					{
						numPids++;
					}
				}
				i++;
				mask=mask>>1;
				if (mask==0)
				{
					dataByte++;
					mask=0x80;
				}
			}
			offset=offset + 0x20; 

			//now handle the configuration PIDS
			if (isPidSupported(0x13))
			{
				cmd[0]=0x01;
				cmd[1]=0x13;

				ret=obdII_send(cmd,2,data,MAX_RESP);
				if (ret)
				{
					pid13=data[0].data[2];
				}
			}
			if (isPidSupported(0x1D))
			{
				cmd[0]=0x01;
				cmd[1]=0x1D;

				ret=obdII_send(cmd,2,data,MAX_RESP);
				if (ret)
				{
					pid1D=data[0].data[2];
				}
			}

		}while(data[0].data[5] & 0x01);
			
	}
   	ncount++;
/*
	printf_P(PSTR("Supported pids: "));
	for (i=0; i<8; i++)
	{
		printf_P(PSTR("%X ",pidSupported[i]);
	}
	printf_P(PSTR("\n\r"));	  
*/
	return numPids;
}

				

	

//gets PID from ECM and returns string. The 
// value returned from function is number of lines
// stored in string
UBYTE obdII_read_PID(UBYTE PID, UBYTE *str, UBYTE nStr,UBYTE *str2, UBYTE nStr2)
{
	OBDII data[MAX_RESP];
	UBYTE cmd[2],ret,i;
	
	cmd[0]=0x01;
	cmd[1]=PID;
	
	ret=obdII_send(cmd,2,data,MAX_RESP);

	if (ret==0)
	{
		return 0;
	}
	
	for(i=0; i<ret; i++)
	{
		if(data[i].data[1]==PID)
			process_PID(data[i].data,str,nStr,str2,nStr2);
		else
		{
			str[0]=0;
			str2[0]=0;
		}
	}
	return 1;
}

/*
UBYTE obdII_pid_get(UBYTE *str, UBYTE line)
{
	OBDII data[MAX_RESP];	
   	UBYTE cmd[2],ret,i,j;
	UBYTE mask,dataByte;
	UBYTE offset,pidNum;
	UBYTE pid;

	offset=0;
	pidNum=0;
  	do {
	  	//first find supported PIDs
		cmd[0]=0x01;
		cmd[1]=offset;

		ret=obdII_send(cmd,2,data,MAX_RESP);

		if(ret==0)
		{
			printf_P(PSTR("No response from ECM for PID support\n\r"));
			return 0;
		}
		if (ret>1)
		{
			//or the data bytes together
			printf_P(PSTR("Num ECMs %d\n\r",ret);
			for(j=0; j<ret; j++)
			{
				for(i=2; i<6; i++)
				{
					data[0].data[i]=data[0].data[i] | data[j].data[i];
				}
			}
		}

		printf_P(PSTR("pids "));
		for(i=0; i<data[0].n;i++)
		{
			printf_P(PSTR("%X ",data[0].data[i]);
		}
		printf_P(PSTR("\n\r"));

		//else lets process PIDs.... 
		i=1+offset;
		mask=0x80;
		dataByte=2;
		while(dataByte<6 && dataByte<data[0].n)
		{
			if (data[0].data[dataByte] & mask)
			{
				if (i!=0x01)
				{
					pidNum++;
				}
				if (pidNum==line)
				{
					obdII_read_PID(i,str,50);
					return i;
				}
			}
			i++;
			mask=mask>>1;
			if (mask==0)
			{
				dataByte++;
				mask=0x80;
			}
		}
		offset=offset + 0x20; 
	}while(data[0].data[5] & 0x01);

	return 0;
}	
			 

//reads PIDs from ECM
UBYTE obdII_pid_test()
{
   	OBDII data[MAX_RESP];	
   	UBYTE cmd[2],ret,i,j;
   	UBYTE str[50];
	UBYTE mask,dataByte;
	UBYTE offset;

	offset=0;
  	do {
	  	//first find supported PIDs
		cmd[0]=0x01;
		cmd[1]=offset;

		ret=obdII_send(cmd,2,data,MAX_RESP);

		if(ret==0)
		{
			printf_P(PSTR("No response from ECM for PID support\n\r"));
			return 0;
		}
		if (ret>1)
		{
			//or the data bytes together
			printf_P(PSTR("Num ECMs %d\n\r",ret);
			for(j=0; j<ret; j++)
			{
				for(i=2; i<6; i++)
				{
					data[0].data[i]=data[0].data[i] | data[j].data[i];
				}
			}
		}

		printf_P(PSTR("pids "));
		for(i=0; i<data[0].n;i++)
		{
			printf_P(PSTR("%X ",data[0].data[i]);
		}
		printf_P(PSTR("\n\r"));

		//else lets process PIDs.... 
		i=1+offset;
		mask=0x80;
		dataByte=2;
		while(dataByte<6 && dataByte<data[0].n)
		{
			if (data[0].data[dataByte] & mask)
			{
				obdII_read_PID(i,str,50);
			}
			i++;
			mask=mask>>1;
			if (mask==0)
			{
				dataByte++;
				mask=0x80;
			}
		}
		offset=offset + 0x20; 
	}while(data[0].data[5] & 0x01);

	return 0;
}
*/	

UBYTE obdII_leds_off()
{
	LED_CAT(0);
	LED_HEATED_CAT(0);
	LED_DTC(0);
	LED_EVAP(0);
	LED_AIR(0);
	LED_O2(0);
	LED_HEATED_O2(0);
	LED_EGR(0);
	LED_MISFIRE(0);
	LED_AC(0);
	LED_FUEL(0);
	LED_COMP(0);
	return 0;
}

UBYTE obdII_leds(OBDII *data_in,UBYTE nResp)
{
	UBYTE i,j;

	OBDII data[1];

	//Logically or multiple ECM responses for Status
	for (i=0; i<nResp; i++)
	{
		for (j=1; j<data_in[i].n; j++)
		{
			data[0].data[j]= data_in[0].data[j] | data_in[i].data[j];
		}
	}

	if (data[0].data[2] & 0x80) 
	{
		MIL_ON=1;
		//LED_DTC(1);
	}
	else
	{
		MIL_ON=0;
		//LED_DTC(0);
	}

	if ((data[0].data[3] & 0x01) && (data[0].data[3] & 0x10))
		LED_MISFIRE(0);
	else
		LED_MISFIRE(1);

	if ((data[0].data[3] & 0x02) && (data[0].data[3] & 0x20))
	{
	   	LED_FUEL(0);
	}
	else
	{
		LED_FUEL(1);	
	}
  
  	if ((data[0].data[3] & 0x04) && (data[0].data[3] & 0x40))
	{
		LED_COMP(0);
	}
	else
	{
		LED_COMP(1);
	}

	if ((data[0].data[4] & 0x01)	&& (data[0].data[5] & 0x01))
		LED_CAT(0);
	else
		LED_CAT(1);
	
	if ((data[0].data[4] & 0x02) && (data[0].data[5] & 0x02))
		LED_HEATED_CAT(0);
	else
		LED_HEATED_CAT(1);
 
 	if ((data[0].data[4] & 0x04) && (data[0].data[5] & 0x04))
		LED_EVAP(0);
	else
		LED_EVAP(1);

	if ((data[0].data[4] & 0x08) && (data[0].data[5] & 0x08))
		LED_AIR(0);
	else
		LED_AIR(1);
   
   	if ((data[0].data[4] & 0x10)	&& (data[0].data[5] & 0x10))
	 	LED_AC(0);
	else
		LED_AC(1);

	if ((data[0].data[4] & 0x20) && (data[0].data[5] & 0x20))
		LED_O2(0);
	else
		LED_O2(1);

	if ((data[0].data[4] & 0x40)	&& (data[0].data[5] & 0x40))
		LED_HEATED_O2(0);
	else
		LED_HEATED_O2(1);

	if ((data[0].data[4] & 0x80) && (data[0].data[5] & 0x80))
		LED_EGR(0);
	else
		LED_EGR(1);
  
	return 0;
}

 
UBYTE obdII_get_num_codes(UBYTE reset)
{
	OBDII data[MAX_RESP];
	UBYTE cmd[2],ret,i; 
	static UBYTE state=0;
	static UDWORD timer;
	static UBYTE nCodes=0;
	static UBYTE ncount=0;


	if(reset)
		ncount=0;
	if (ncount>50 || ncount==0)
	{
		ncount=0;

		//first find number of codes
		cmd[0]=0x01;
		cmd[1]=0x01;

		ret=obdII_send(cmd,2,data,MAX_RESP);

		if (ret==0 || ret>MAX_RESP)
		{
			printf_P(PSTR("No response for number of codes %d\n\r"),ret);
			return 0;
		}

		obdII_leds(data,ret);

		//process data
		nCodes=0;
		for (i=0; i<ret; i++)
		{
			if (data[i].data[0]==0x41)
			{
				nCodes=nCodes+(data[i].data[2] & 0x7F);
			}
		}
	}
	ncount++;

	if(nCodes && MIL_ON==0)
	{
		if (timer!=getSeconds())
		{
			state=!state;
			LED_DTC(state);
			timer=getSeconds();
		}
	}else
	{
		LED_DTC(MIL_ON);
	}	

	return nCodes;
}	 

UWORD obdII_get_code(UBYTE index, UBYTE nCodes)
{
	OBDII data[MAX_RESP];
	UBYTE cmd[2],ret,i,j,k,l;
	UWORD code;

	
	if (index>nCodes) //Opps someone passed bad parameters
	{
		return 0;
	}
	cmd[0]=0x03;
	ret=obdII_send(cmd,1,data,MAX_RESP);

	if (ret==0 || ret>MAX_RESP)
	{
		printf_P(PSTR("No response for codes %d\n\r"),ret);
		return 0;
	}
	//process codes
	
	if(index<1)
		return 0;
	l=0;
	for (i=0; i<ret; i++)
	{
		
		k=1;
		j=0;
		while (k<data[i].n)
		{
			if (data[i].data[0]==0x43)
			{
				code=(UWORD)((data[i].data[1+(j*2)] & 0xC0)>>6)*10000;
				code=code+(UWORD)((data[i].data[1+(j*2)] & 0x30)>>4)*(UWORD)1000;
				code=code+ (UWORD)((data[i].data[1+(j*2)] & 0x0F))*(UWORD)100;
				code=code+ (UWORD)((data[i].data[2+(j*2)] & 0xF0)>>4)*(UWORD)10;
				code=code+ (UWORD)((data[i].data[2+(j*2)] & 0x0F));
				//printf_P(PSTR("code is %lu\n\r",code);
				j++;
				if (code>0)
				{	
					l++;
					if (l==index)
					{
						return code;
					}				
				}
				k=k+2; 
			}else
			{
				k=data[i].n;
			}
			
		}//while
	}
	return 0;

}
  

//we need a function to read codes
UBYTE obdII_codes()
{
	UBYTE i;
	UBYTE nCodes;

	//first find number of codes
	nCodes=obdII_get_num_codes(0);
  	printf_P(PSTR("Found %d codes\n\r"),nCodes);

	for (i=0; i<nCodes; i++)
	{
		printf_P(PSTR("Code %u\n\r"),obdII_get_code(i,nCodes));
	}
   	printf_P(PSTR("Codes Done!\n\r"));
	return nCodes;
} 


//gets the status string by index/line number
UBYTE obdII_get_status(UBYTE *str, UBYTE line)
{
	OBDII data[MAX_RESP];
	static UBYTE piddata[7];
	static UBYTE count=0;	
	UBYTE cmd[2],ret,i,j;
	
	if (line>NUM_STATUS_LINES)
	{
		sprintf_P(str, PSTR("STATUS ERROR"));
		return 0;
	}
	//read the status bits
	if (count==0 || count>50)
	{
		cmd[0]=0x01;
		cmd[1]=0x01;

		ret=obdII_send(cmd,2,data,MAX_RESP);

		if (ret==0 || ret>MAX_RESP)
		{
			printf_P(PSTR("No response for status %d\n\r"),ret);
			count=0;
			return 0;
		}

		//Logically or multiple ECM responses for Status
		for (i=0; i<ret; i++)
		{
			for (j=1; j<data[i].n; j++)
			{
				piddata[j]= data[0].data[j] | data[i].data[j];
			}
		}
	}
	//process data
	switch(line)
	{
		case 1:
		  if (piddata[2] & 0x80)
		  {
		  	sprintf_P(str, PSTR("MIL: ON"));
		  }else
		  {
		  	sprintf_P(str, PSTR("MIL: OFF"));
		  }
		  return 0;
		case 2:
			if (piddata[3] & 0x01)
			{
				if (piddata[3] & 0x10)
				{
					sprintf_P(str, PSTR("Missfire: NotRdy"));
				}else
				{
					sprintf_P(str, PSTR("Missfire: Ready"));
				}
			}else
			{
				sprintf_P(str, PSTR("Missfire: N/A"));
			}
			return 0;
		 case 3:
			if (piddata[3] & 0x02)
			{
				if (piddata[3] & 0x20)
				{
					sprintf_P(str, PSTR("Fuel Sys: NotRdy"));
				}else
				{
					sprintf_P(str, PSTR("Fuel Sys: Ready"));
				}
			}else
			{
				sprintf_P(str, PSTR("Fuel Sys: N/A"));
			}
			return 0;
		 case 4:
			if (piddata[3] & 0x04)
			{
				if (piddata[3] & 0x40)
				{
					sprintf_P(str, PSTR("Comp. Mon: NotRdy"));
				}else
				{
					sprintf_P(str, PSTR("Comp. Mon: Ready"));
				}
			}else
			{
				sprintf_P(str, PSTR("Comp. Mon: N/A"));
			}
			return 0;
		case 5:
			if (piddata[4] & 0x01)
			{
				if (piddata[5] & 0x01)
				{
					sprintf_P(str, PSTR("Catalyst: NotRdy"));
				}else
				{
					sprintf_P(str, PSTR("Catalyst: Ready"));
				}
			}else
			{
				sprintf_P(str, PSTR("Catalyst: N/A"));
			}
			return 0;
		case 6:
			if (piddata[4] & 0x02)
			{
				if (piddata[5] & 0x02)
				{
					sprintf_P(str, PSTR("Heated Cat: NotRdy"));
				}else
				{
					sprintf_P(str, PSTR("Heated Cat: Ready"));
				}
			}else
			{
				sprintf_P(str, PSTR("Heated Cat: N/A"));
			}
			return 0;
		case 7:
			if (piddata[4] & 0x04)
			{
				if (piddata[5] & 0x04)
				{
					sprintf_P(str,PSTR("EVAP: NotRdy"));
				}else
				{
					sprintf_P(str,PSTR("EVAP: Ready"));
				}
			}else
			{
				sprintf_P(str,PSTR("EVAP: N/A"));
			}
			return 0;
		case 8:
			if (piddata[4] & 0x08)
			{
				if (piddata[5] & 0x08)
				{
					sprintf_P(str,PSTR("Sec. Air: NotRdy"));
				}else
				{
					sprintf_P(str,PSTR("Sec. Air: Ready"));
				}
			}else
			{
				sprintf_P(str,PSTR("Sec. Air: N/A"));
			}
			return 0;
		case 9:
			if (piddata[4] & 0x10)
			{
				if (piddata[5] & 0x10)
				{
					sprintf_P(str,PSTR("A/C Sys: NotRdy"));
				}else
				{
					sprintf_P(str,PSTR("A/C Sys: Ready"));
				}
			}else
			{
				sprintf_P(str,PSTR("A/C Sys: N/A"));
			}
			return 0;
		case 10:
			if (piddata[4] & 0x20)
			{
				if (piddata[5] & 0x20)
				{
					sprintf_P(str,PSTR("O2 Sensor: NotRdy"));
				}else
				{
					sprintf_P(str,PSTR("O2 Sensor: Ready"));
				}
			}else
			{
				sprintf_P(str,PSTR("O2 Sensor: N/A"));
			}
			return 0;
		case 11:
			if (piddata[4] & 0x40)
			{
				if (piddata[5] & 0x40)
				{
					sprintf_P(str,PSTR("O2 Heater: NotRdy"));
				}else
				{
					sprintf_P(str,PSTR("O2 Heater: Ready"));
				}
			}else
			{
				sprintf_P(str,PSTR("O2 Heater: N/A"));
			}
			return 0;
		case 12:
			if (piddata[4] & 0x80)
			{
				if (piddata[5] & 0x80)
				{
					sprintf_P(str,PSTR("EGR Sys: NotRdy"));
				}else
				{
					sprintf_P(str,PSTR("EGR Sys: Ready"));
				}
			}else
			{
				sprintf_P(str,PSTR("EGR Sys: N/A"));
			}
			return 0;
	}
	sprintf_P(str,PSTR("Status N/A"));

	return 0;
}
	
UBYTE obdII_run(BYTE index, UBYTE reset)
{
	static INT line=0;
	UBYTE temp,temp2;
	UBYTE str[20];
	UBYTE str2[20];
	UBYTE str3[20];
	UBYTE str4[20];
	UBYTE str5[20];

	UBYTE nCodes;
	UBYTE nPids=0,i,pid;
	static UBYTE lastPid=0;
	static UWORD lastcode=0;

	//first find number of codes
	nCodes=obdII_get_num_codes(reset);
	if (obdError==0)
	{
	   //get number of PIDS
	   nPids=PidInit();	
	   if (isPidSupported(0x13))
			nPids--;
	   if (isPidSupported(0x1d))
			nPids--;

	}
  
	if (obdError)
	{
		//LCD_clear();
		sprintf_P(str,PSTR("No Communications"));	
		LCD_print1(str,0);
		sprintf_P(str,PSTR("Turn Key On"));
		LCD_print2(str,0);
		lastcode=0;
		obdII_leds_off();
		return 0;
	}
	
	if(index!=0)
		lastcode=0;

	line=line+index;
	if (line<0)
	{
		line=0;
	}
	
	sprintf_P(str,PSTR("Line=%d"),line);
	if (line>(INT)(nCodes+NUM_STATUS_LINES+nPids))
	{
		line=nCodes+NUM_STATUS_LINES+nPids;
	}


	if (line<=0)
	{
		line=0;
		sprintf_P(str,PSTR("NumCodes %u"),nCodes);
		LCD_print1(str,0);
		sprintf_P(str,PSTR("Scroll to view"));
		LCD_print2(str,0);
		return 0;
	}


	temp=line;
	if (temp<=nCodes)
	{
		UWORD code;

		//get the code 		
		code=obdII_get_code(temp,nCodes);
		//since the code is scrolling we can not update every loop
		//so let's update when code changes...
		//printf_P(PSTR("code %lu %u\n\r",code,temp);
		if (lastcode!=code && code!=0)
		{
			//printf_P(PSTR("printing code %lu %u\n\r",code,temp);
			PcodePrint(code);
			lastcode=code;
		}
		//printf_P(PSTR("printing code done\n\r"));
		//PcodePrint(temp+100);
		return 0;
	}else
	{
		lastcode=0;
	}

	//Now lets handle the status monitors
	temp=line-nCodes;
	if (temp<NUM_STATUS_LINES)
	{
		//printf_P(PSTR("status temp=%u\n\r",temp);
		obdII_get_status(str,temp);
		LCD_print1(str,0);
		obdII_get_status(str,temp+1);
		LCD_print2(str,0);
		return 0;
	}

	//now lets read the data stream... 
	temp=temp-NUM_STATUS_LINES+1;
	//printf_P(PSTR("PID line is %d\n\r",temp);
	pid=0x02;
	for(i=0; i<temp; i++)
	{
		temp2=getNextPid(pid);
		if (temp2>0)
			pid=temp2;
	}
	//printf_P(PSTR("PID is %u\n\r",pid);

	//get the labels for the PIDs
	pidLabels(pid,str,20,str2,20);
	temp2=0;
	if (str2[0]==0)
	{
		//printf_P(PSTR("reading next pid\n\r"));
		temp2=getNextPid(pid);
		//printf_P(PSTR("next pid is %u\n\r",temp2);
		if (temp2!=0)
		{
			pidLabels(temp2,str2,20,str5,20);
		}
	}
	////printf_P(PSTR("Temp2 is %u\n\r",temp2);
	if (pid!=lastPid)
	{
		LCD_print1(str,0);
		LCD_print2(str2,0);
		lastPid=pid;
	}
	obdII_read_PID(pid,str3,20,str4,20);
	//printf_P(PSTR("str4 is %s\n\r",str4);
	if(temp2!=0)
	{
		//printf_P(PSTR("reading second pid value %u\n\r",temp2);
		obdII_read_PID(temp2,str4,20,str5,20);
	}
	str5[0]=0;
	sprintf_P(str5,PSTR("%s%s"),str,str3);
	str[0]=0;
	sprintf_P(str,PSTR("%s%s"),str2,str4);
	//printf_P(PSTR("str is %s\n\r",str);
	LCD_print1(str5,0);
	LCD_print2(str,0);
/*				
	if (temp<=nPids)
	{
		obdII_pid_get(str, temp) ;
		LCD_print1(str,0);
		obdII_pid_get(str,temp+1);
		LCD_print2(str,0);
	}
*/
    return 0;	

}	

//does CRC calculations
UBYTE crc(UBYTE *data, UBYTE len)
{
	UBYTE result;
	UBYTE i;
	UBYTE mask;
	UBYTE j;
	UBYTE temp;
	UBYTE poly;

	result=0xFF;
	for(i=0; i<len; i++)
	{
		mask=0x80;
		temp=data[i];
		for(j=0; j<8; j++)
		{
			if(temp & mask)	  //bit is 1
			{
				poly=0x1c;	
				if(result & 0x80)
				{
					poly=1;
				}
				result= ((result<<1) | 0x01) ^ poly;

			}else
			{
			 	poly=0;
				if(result & 0x80)
				{
					poly=0x1D;
				}  
				result= ((result<<1)) ^ poly;

			}
			mask=mask>>1;
		}
	}
	return ~result;
}


//does crc/checksum error checking
//0 no errors
UBYTE check_errors(UBYTE *data, UBYTE len, UBYTE connection)
{
	//next lets check the CRC and/or check sum
	if (connection==VPW || connection==PWM)
	{
		//we have a CRC response
		UBYTE temp;
		temp=crc(data,len-1);
		if (temp!=data[len-1])
		{
			return 1;
		}
	}else
	{
		//ISO
		//assume checksum
		UBYTE temp;
		temp=checksum(data,len-1);
		if (temp!=data[len-1])
		{
			return 2;
		}
	}
	return 0;
}

//Lets assume we have some data and now we need to parse it 
// into different ECMS. 
UBYTE process(OBDII *resp, UBYTE num_resp, UBYTE *data, UBYTE num_data, UBYTE connection)
{
	UBYTE byte1=0;
	UBYTE byte2=0;
	UBYTE done,i,n;
	//check vaild input
	if (num_data<5 || num_resp<1 || connection==UNKNOWN)
	{
		printf_P(PSTR("process error %d\n\r"),num_data);
		return 0;
	}

	if (connection==VPW || connection==ISO)
	{
		//first two bytes of field is 0x48 0x6B
		byte1=0x48;
		byte2=0x6B;
	}
	if (connection==PWM)
	{
		//first two bytes of field are 0x41 0x6B
		byte1=0x41;
		byte2=0x6B;
	} 

	done=0;
	i=0;
	n=0; //index in the OBDII struct
	while(!done)
	{
		//check first two bytes
		if (data[i]==byte1 && data[i+1]==byte2)
		{
			UBYTE j;
			//we can process the data for
			//first we need to know the length. 
			//to find length check CRC. 
			//printf_P(PSTR("FOUND data i=%d\n\r",i);
			for (j=5; j<=(num_data); j++)
			{
				//printf_P(PSTR("j=%d num_data=%d\n\r",j,num_data);
				//printf_P(PSTR("data[j]=%X data[j+1]=%X\n\r",data[j],data[j+1]);
				if( (data[j]==byte1 && data[j+1]==byte2) || j==num_data )
				{
					//printf_P(PSTR("checking %d %d\n\r",i,j);
					if (check_errors(&data[i],j-i,connection)==0)
					{
						UBYTE l;
						//we have vaild data
						//printf_P(PSTR("Valid data %d %d\n\r",i,j);
						resp[n].addr=data[i+2];
						resp[n].n=j-4;
						for(l=3; l<j; l++)
						{
							resp[n].data[l-3]=data[i+l];
						}
						//i=i+j-1;
						n++;
					    if (n>num_resp || i>=num_data)
						{
							return n;
						}
					   //	j=(num_data+1);

					}
				}
			
			}
		}
		i++;
		if ((i+3)>=num_data)
		{
			done=1;
		}
	}
/*
	if (n==0)
	{
		printf_P(PSTR("Malformed data: "));
		for(i=0; i<num_data; i++)
		{
			printf_P(PSTR("%X ",data[i]);
		}
		printf_P(PSTR("\n\r\n\r"));
	}
*/
	return n;
}


	
//we need some way to read the codes and data from 
//obdII car
UBYTE obdII_send2(UBYTE *msg, UBYTE len_msg, OBDII *resp, UBYTE num_resp)
{
	//well we need to
	UBYTE resp_str[SIZE_RESP_STR];
	UBYTE ret,temp;
	static UBYTE cnt=0;

	ret=0;
	if (connect!=UNKNOWN)
	{
		//lets try last connection
		if (connect==ISO)
		{
			ret=iso_send(resp_str,SIZE_RESP_STR,msg,len_msg,0x33);

		}/*else if (connect==PWM)
		{
			ret=pwm_send(resp_str,SIZE_RESP_STR,msg,len_msg);
		}else if (connect==VPW)
		{
			ret=vpw_send(resp_str,SIZE_RESP_STR,msg,len_msg);
		} */
		else
		{
			connect=UNKNOWN;
			ret=0;
		}
		if(ret)
		{
		   //	UBYTE i,j;
			//check data and process
			temp=process(resp,num_resp,resp_str,ret,connect);
/*
			for(j=0; j<temp; j++)
			{
				printf_P(PSTR("ECM %X returned ",resp[j].addr);
				for(i=0; i<resp[j].n;i++)
				{
					printf_P(PSTR("%X ",resp[j].data[i]);
				}
				printf_P(PSTR("\n\r"));
			}
*/
			return temp;
			//return 0;
		}
		cnt++;
		if (cnt<5)
			return 0;
		cnt=0;
	}
	
	//else lets try all possible connections
	printf_P(PSTR("Checking ISO\n\r"));	
   	ret=iso_send(resp_str,SIZE_RESP_STR,msg,len_msg,0x33);
	if (ret)
	{
		connect=ISO;
	}
	/*
   	if (ret==0)
	{
		printf_P(PSTR("Checking PWM\n\r"));	
		//LCD_clear();
		//sprintf_P(str, "Checking PWM"));
		//LCD_print1(str,0);

		connect=PWM;
		ret=pwm_send(resp_str,SIZE_RESP_STR,msg,len_msg);

	}
	if (ret==0)
	{
		printf_P(PSTR("Checking VPW\n\r"));
		//LCD_clear();
		//sprintf_P(str, "Checking VPW"));
		//LCD_print1(str,0);

		connect=VPW;
		ret=vpw_send(resp_str,SIZE_RESP_STR,msg,len_msg);
	}
	*/
	if (ret)
	{
		temp=process(resp,num_resp,resp_str,ret,connect);
/*
		for(j=0; j<temp; j++)
		{
			printf_P(PSTR("ECM %X returned ",resp[j].addr);
			for(i=0; i<resp[j].n;i++)
			{
				printf_P(PSTR("%X ",resp[j].data[i]);
			}
			printf_P(PSTR("\n\r"));
		}
*/
		return temp;

	}else
	{
		connect=UNKNOWN;
		return 0;
	}

}
	
UBYTE obdII_send(UBYTE *msg, UBYTE len_msg, OBDII *resp, UBYTE num_resp)
{
	UBYTE i,ret;
	UBYTE cnt;
	cnt=5;
	if (connect==PWM)
	{
		cnt=20;
	}
	i=0;
	while(i<cnt)
	{
		ret=obdII_send2(msg,len_msg,resp,num_resp);
		if(ret)
		{
			obdError=0;
			return ret;
		}
		i++;
	}
	//connect=UNKNOWN;
	obdError=1;
	return 0;
}
