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

#include "strio.h"
#include "datatypes.h"


//depending on your complier implementation 
//you may need to change these. It makes a difference
//how your data is passed to functions, for example some 
//compilers pass chars as 16bit ints.
#define SPRINT_8BIT		char
#define SPRINT_8BIT_PASS int
#define SPRINT_16BIT	int
#define SPRINT_32BIT	long
#define SPRINT_FLOAT 	float

#define FLAG_NEG		0x01  //%-
#define FLAG_POS		0x02  //%+
#define FLAG_0			0x04  //%0
#define FLAG_ASTR		0x08  //%*  read next 8bits that specifies width
#define FLAG_UNSIGNED   0x10  //%u
#define FLAG_DOT		0x20  //%.  seen a decimal point
#define FLAG_SPACE		0x40  //% space padding
#define FLAG_SIGN		0x80  //Value needs positive printed

#define SIZE_8BIT		8	//%d
#define SIZE_16BIT		16  //%h
#define SIZE_32BIT		32  //%l or %L

#define DEAFULT_SIZE SIZE_16BIT		//set this to the default size for "%x" or "%d"	
#define MAX_CHARS	   MAX_STRING	//maximum number or string width passed to printf as variable length argument

/*
//7-20-03
//Converts a string to float
//TODO rewrite this using atoi rountine
FLOAT atof(CHAR *str)
{
	INT i;
	UINT dec; 
	UINT len;
	FLOAT result;
	FLOAT temp;
	CHAR *ptr;
	CHAR str2[100];
	
	//find decimal point
	ptr=str;
	i=0;
	while(*ptr!=0)
	{
		if(*ptr=='.')
		{
			dec=i;
		}
		i++;
	}
	len=i;
	
	if(len==0 | len>100)
		return 0.0;
	
	//OK now copy string to new buffer
	strcpy(str,str2);
	
	//set the decimal point to zero;
	str2[dec]=0;
	
	//Do the mantisa
	result=0;
	temp=1;
	for(i=0; i<dec; i++)
	{
		CHAR c;
		c=str[dec-1-i]-'0';
		if (c>=0 && c<=9)
		{
			result=result+(c*temp);
		}
		temp=temp*10;
	}
	
	//Do the fractional
	temp=1.0/10.0;
	for(i=dec+1; i<len; i++)
	{
		CHAR c;
		c=str[dec-1-i]-'0';
		if (c>=0 && c<=9)
		{
			result=result+(c*temp);
		}
		temp=temp*(1.0/10.0);
	}	
	
	return result;
}
*/		
	
	



/******************************************************************* 
 *  FUNCTION :tbs_ltoa
 *
 *  AUTHOR: Trampas Stern 
 *
 *  DATE : 2/11/2003	1:45:34 PM
 *
 *  FILENAME: strio.c
 *
 *  PARAMETERS:
 *
 *  DESCRIPTION: Converts a long to a decimal value
 *
 *  RETURNS:
 *
*******************************************************************/

int tbs_ltoa(CHAR *str, long lval, int sign)
{
	unsigned long k;
	int i,j;		   		
	//unsigned long temp;

	j=0;
	if(sign)
	{
		i=0;
		if (lval<0)
		{
			*str++='-';
			j=j+1;
			lval=-lval;
		}
		//lets implement this in a simple manner
		for (k=1000000000; k>=1; k=k/10)
		{
			char t;
			t=(lval/k);
			if((t>0) || (i>0))
			{
				lval=lval-(t*k);
				str[i++]=t+'0';
			}
			if(i>MAX_STRING) return 255;
		}
	}else
	{
	  	i=0;
		//temp=(unsigned long)lval;		
		for (k=1000000000; k>=1; k=k/10)
		{
			char t;
			t=(lval/k);
			if((t>0) || (i>0))
			{
				lval=lval-(t*k);
				str[i++]=t+'0';
			}
			if(i>MAX_STRING) return 255;

		}
	}
	if (i==0)
		str[i++]='0';
	str[i++]=0;
	return i+j;
}


/*******************************************************************
 *  FUNCTION: ftoa
 *  AUTHOR 		= 	TRAMPAS STERN
 *  FILE 		=	strio.c
 *  DATE		=   2/6/2003  4:27:14 PM
 *
 *  PARAMETERS: long,*str, int count
 *
 *  DESCRIPTION: Convets an float to string
 *			format 'f', 'E', or 'e'
 *		
 *
 *  RETURNS: 
 *
 * NOTE this code was found on the web and modified to actually work
 *******************************************************************/
int ftoa (float x, CHAR *str, char  prec, char format)
{

	SPRINT_16BIT ie, i, k, ndig, fstyle;
	double y;
	CHAR *start;

	start=str;
	
	//based on percission set number digits 
	ndig=prec+1;
	if (prec<0)
		ndig=7;
	if (prec>22)
		ndig=23;

	fstyle = 0;	//exponent 'e'
	if  (format == 'f' || format == 'F') 
		fstyle = 1;	//normal 'f'
	if (format=='g' || format=='G')
		fstyle=2;
  		
	ie = 0;
	/* if x negative, write minus and reverse */
	if ( x < 0)
	{
	  *str++ = '-';
	  x = -x;
	}

	//if (x<0.0) then increment by 10 till betwen 1.0 and 10.0
	if (x!=0.0)
	{
		while (x < 1.0)
		{
		  x =x* 10.0;
		  ie--;
		}
	}

   	//if x>10 then let's shift it down
	while (x >= 10.0)
	{
		x = x*(1.0/10.0);
		ie++;
	}

	if (ABS(ie)>MAX_MANTISA)
	{
		if (fstyle==1)
		{
			fstyle=0;
			format='e';
			//ie=2;
		}
	}
	

	/* in f format, number of digits is related to size */
	if (fstyle) 
		ndig =ndig + ie; 
		
	if(prec==0 && ie>ndig && fstyle)
	{
		ndig=ie;
	}
		
	/* round. x is between 1 and 10 and ndig will be printed to
	   right of decimal point so rounding is ... */
	y=1;
	for (i = 1; i < ndig; i++)	//find lest significant digit
	  y = y *(1.0/10.0); 		//multiply by 1/10 is faster than divides

	x = x+ y *(1.0/2.0);			//add rounding

	/* repair rounding disasters */
	if (x >= 10.0) 
	{
		x = 1.0; 
		ie++;
		ndig++;
	} 
	
	//check and see if the number is less than 1.0
	if (fstyle && ie<0)
	{
		*str++ = '0'; 
		if (prec!=0)
			*str++ = '.';
		if (ndig < 0) 
			ie = ie-ndig; /* limit zeros if underflow */
		for (i = -1; i > ie; i--)
			*str++ = '0';
	}

	//for each digit 
	for (i=0; i < ndig; i++)
	{
		float b;
		k = x;						//k = most significant digit
		*str++ = k + '0';			//output the char representation
		if (((!fstyle && i==0) || (fstyle && i==ie)) && prec!=0)  
			*str++ = '.';			//output a decimal point
		b=(float)k;
		//multiply by 10 before subtraction to remove 
		//errors from limited number of bits in float. 
		b=b*10.0;
		x=x*10.0;
		x =x - b;				//subtract k from x
		//b=x+b;
		//x =x* 10.0;					//get next digit				
	}

/* now, in estyle,  put out exponent if not zero */
	if (!fstyle && ie != 0)
	{
		*str++ = format;
		if (ie < 0)		//if number has negative exponent
		{
			ie = -ie;
			*str++ = '-';
		}

		//now we need to convert the exponent to string
		for (k=1000; k>ie; k=k/10);		//find the decade of exponent

		for (; k > 0; k=k/10)
		{
			char t;
			t=DIV(ie,k);
			*str++ = t + '0';
			ie = ie -(t*k);
		}

	}
	*str++ = '\0';
	return (str-start);	//return string length
}


/*******************************************************************
 *  FUNCTION: hextoa
 *  AUTHOR 		= 	TRAMPAS STERN
 *  FILE 		=	strio.c
 *  DATE		=   2/6/2003  5:16:17 PM
 *
 *  PARAMETERS: 
 *
 *  DESCRIPTION: converts a long to hexdecimal
 *
 *  RETURNS: 
 *
 *
 *******************************************************************/
int hextoa(CHAR *str, long data, char lowerCase)
{
	int i,k;
	char c;
		
	i=0;
	
	for(k=0; k<8; k++)
	{
	   c=(char)(data>>((7-k)*4)) & 0x0F;
	   if (c<10)
	   {
			if((i!=0) || (c>0))
			{
				str[i++]=c+'0';
			 }
		}else
			if (lowerCase)
				str[i++]=c+'a'-10;
			else
				str[i++]=c+'A'-10;
	  if(i>MAX_STRING) return 255;

	 }
	 if (i==0)
		str[i++]='0';
	 str[i++]=0;
	 return i;
}

/*******************************************************************
 *  FUNCTION: octtoa
 *  AUTHOR 		= 	TRAMPAS STERN
 *  FILE 		=	strio.c
 *  DATE		=   2/6/2003  5:16:17 PM
 *
 *  PARAMETERS: 
 *
 *  DESCRIPTION: converts a long to an ascii octal
 *
 *  RETURNS: 
 *
 *
 *******************************************************************/
int octtoa(CHAR *str, unsigned long data)
{
	long k;
	char c;
	int i;
		
	i=0;
	
	for(k=1073741824; k>data; k=k>>3);

	while (k>0)
	{
		c=data/k;
		str[i++]=c + '0';
		data=data-(c*k);
		k=k>>3;		//k=k/8
		if(i>MAX_STRING) return 255;

	}
	if (i==0)
		str[i++]='0';

	str[i++]=0;
	return i;
}





/******************************************************************* 
 *  FUNCTION :sprintf
 *
 *  AUTHOR: Trampas Stern 
 *
 *  DATE : 2/11/2003	2:49:36 PM
 *
 *  FILENAME: strio.c
 *
 *  PARAMETERS: Does a printf from string in rom
 *
 *  DESCRIPTION: impelments sprintf
 * 				%[flag] [width].[precision] type
 *
 *			flags:
 *				+   - print sign
 *				' ' - padd using spaces - default
 *				0	- padd left with zeros
 *				-   - left align the result	
 *              *   - read width as next char, printf( "%*.2f", char_width, float_value); 
 *
 *			width:
 *				The minimual width the parameter should expand to. May excced but never less.
 *
 *			Precision:
 *				floating point - number of digits left of the decimal point
 *				types x,X,d    - No effect
 *				strings    	   - Maximum number chars read from pointer	parameter
 *
 *			types:
 *				x 	- lower case hex
 *				X 	- upper case hex
 *              o   - octal
 *              d 	- signed decimal value
 *              s 	- char * string 
 *              e   - floating point in expodential format
 *				E   - floating point in expodential format
 *             	f   - floating point 
 *
 *			
 *
 *  RETURNS: # chars printed
 *
 *
 *******************************************************************/
int sprintf_va(CHAR *str,CHAR *fmt , va_list ap)
{
	//va_list ap;
	CHAR *p2, *pgm;
	SPRINT_8BIT c;
	static char str_temp[MAX_CHARS];
	CHAR *temp;
	SPRINT_16BIT ival=0;
	SPRINT_32BIT lval;
	SPRINT_FLOAT fval;
	int flags;
	int size;
	int width;
	int precision; 
	int i;
	int done;
	//char length;

	i=0;

	//va_start(ap, fmt);
	for(pgm=fmt; *pgm; pgm++)
	{
		if(i>(MAX_STRING-1))
		{
			str[MAX_STRING-1]=0;
			str[MAX_STRING/2]=0;
			//log_printf("ERROR: string overflow");
			//log_printf("%s",str);
			HALT();
			return 255;
		}

		if(*pgm!='%')
		{
		 	str[i++]=(*pgm);
		 	continue;
		}
		width=0;
		precision=0;
		done=0;
		flags=0;
		size=DEAFULT_SIZE;
		temp=str_temp;
		while (!done)
		{
			switch(*++pgm)	  //we have a %
			{
				//print char as decimal
				case 'd':
					switch(size)
					{
					
						case SIZE_16BIT:
							ival=va_arg(ap,SPRINT_16BIT);
							if ((flags & FLAG_POS) && ((ival>0) || (flags & FLAG_UNSIGNED)))
									flags=flags | FLAG_SIGN;
							if (flags & FLAG_UNSIGNED)
								ival=tbs_ltoa(temp,(unsigned long)(unsigned SPRINT_16BIT)ival,0);
							else
								ival=tbs_ltoa(temp,ival,1);	
							break;
						case SIZE_32BIT:
							lval=va_arg(ap,SPRINT_32BIT);
							if ((flags & FLAG_POS)  && ((lval>0) || (flags & FLAG_UNSIGNED)))
									flags=flags | FLAG_SIGN;
							if (flags & FLAG_UNSIGNED)
								ival=tbs_ltoa(temp,(unsigned SPRINT_32BIT)lval,0);
							else
								ival=tbs_ltoa(temp,lval,1);	
							break;
						//case SIZE_8BIT:
						default:
							c=va_arg(ap,SPRINT_8BIT_PASS);
							if ((flags & FLAG_POS) && ((c>0) || (flags & FLAG_UNSIGNED)))
									flags=flags | FLAG_SIGN;
							if (flags & FLAG_UNSIGNED)
								ival=tbs_ltoa(temp,(unsigned long)(unsigned SPRINT_8BIT)c,0);
							else
								ival=tbs_ltoa(temp,c,1);
							break;
					}
					done=2;
					break;
				
				//We are requesting to print ASCII char
				case 'c':
					c=va_arg(ap,SPRINT_8BIT_PASS);
					str[i++]=(c);
					done=1;
					break;

				//print a char in hex
				case 'X':
				case 'x':
					switch(size)
					{
						case SIZE_16BIT:
							ival=va_arg(ap,SPRINT_16BIT);
							ival=hextoa(temp,(unsigned SPRINT_16BIT)ival,*pgm=='x');
							break;
						case SIZE_32BIT:
							lval=va_arg(ap,SPRINT_32BIT);
							ival=hextoa(temp,(unsigned SPRINT_32BIT)lval,*pgm=='x');
							break;
						//case SIZE_8BIT:
						default:
							c=va_arg(ap,SPRINT_8BIT_PASS);
							ival=hextoa(temp,(unsigned SPRINT_8BIT)c,*pgm=='x');
							break;
					}
					done=2;		
					break;
				case 'o':
					switch(size)
					{
						case SIZE_16BIT:
							ival=va_arg(ap,SPRINT_16BIT);
							ival=octtoa(temp,(unsigned SPRINT_16BIT)ival);
							break;
						case SIZE_32BIT:
							lval=va_arg(ap,SPRINT_32BIT);
							ival=octtoa(temp,(unsigned SPRINT_32BIT)lval);
							break;
						//case SIZE_8BIT:
						default:
							c=va_arg(ap,SPRINT_8BIT_PASS);
							ival=octtoa(temp,(unsigned SPRINT_8BIT)c);
							break;
					}
					done=2;		
					break;
				//print a char string 
				case 's':					
					p2=va_arg(ap,char *);
					if (precision==0 || precision>MAX_CHARS)
							precision=MAX_CHARS;	//limit chars to prevent overflow
					{
						ival=0;
						while(*p2 && precision--)
							temp[ival++]=(*p2++);
					}
					ival++;
					done=2;
					break;

				//we have a unsigned request
				case 'u':
					flags=flags | FLAG_UNSIGNED;
					break;
							
				//we have 16bit data type
				case 'h':
					 size=SIZE_16BIT;
					 break;

				//We have a long data type request
				case 'l':
			 		 size=SIZE_32BIT;
					 break;
				case '0':
					 //check to see if it is the first 0
					 if(width==0 && precision==0 && ((flags & FLAG_DOT)==0))
						 flags=flags | FLAG_0;
					 else
					 {
						 if((flags & FLAG_DOT)) //we have seen a decimal point
						 {
							precision=precision*10;
						 }else
						 {
							width=width*10;
						 }
					 }
					 break;
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
					if((flags & FLAG_DOT)) //we have seen a decimal point
					{
						precision=precision*10+(*pgm-'0');
					}else
					{
						width=width*10+(*pgm-'0');
					}
					break;
				case '-':
					flags=flags | FLAG_NEG;
					break;
				case '+':
					flags=flags | FLAG_POS;
					break;
				case '*':
					flags=flags | FLAG_ASTR; //we dont need to do this
					width==va_arg(ap,SPRINT_8BIT_PASS);
					break;
				case ' ':
					flags=flags | FLAG_SPACE;
					break;
				case '.':
					flags=flags | FLAG_DOT;
					break;
				case 'f':
				case 'e':
				case 'E':
					fval=va_arg(ap,double);
					if ((flags & FLAG_POS) && fval>=0)	//does user want '+'
						flags=flags | FLAG_SIGN;
					ival=ftoa (fval, temp, precision, *pgm);
					done=2;
					break; 
			   
			   default:
					str[i++]=(*pgm);
					done=1;
					break;
			} //switch

			// do the sign and shifting of the data to the desired widths
			if(done==2)
			{
				ival=ival-1; //lose the '\0'
				if (flags & FLAG_NEG)
				{
					//we are to left align the data and follow up with spaces
					if (flags & FLAG_SIGN)		//check for posistive sign
							str[i++]='+';
					for (c=0; c<width || c<ival; c++)
						if(c<ival)
							str[i++]=temp[(int)c];
						else
							str[i++]=' ';
				} else
				{
					if (flags & FLAG_0)
					{
						//if we are to prefix with zeros put sign out first
						if (flags & FLAG_SIGN)	//check for posistive sign
							str[i++]='+';	
						if (temp[0]=='-')		//check for negative sign
						{
							str[i++]=*temp++;   //increment temp passed sign
							ival--;
						}
						//now put out the zeros
						for(c=0; c<width-ival; c++)	
							str[i++]='0';
					}else
					{
						//we are to prefix with spaces
						for(c=0; c<width-ival; c++)	
							str[i++]=' ';
						if (flags & FLAG_SIGN)
							str[i++]='+';
					}
						
					for(c=0; c<(ival); c++)
						str[i++]=temp[(int)c];
				}
			}

		}//while
	} //for
	str[i++]=0;
		
	return i; //return string count
}



/*******************************************************************
 *  FUNCTION: sprintf
 *  AUTHOR 		= 	TRAMPAS STERN
 *  FILE 		=	strio.c
 *  DATE		=   2/6/2003  4:44:47 PM
 *
 *  PARAMETERS: str, data
 *
 *  DESCRIPTION: impelments sprintf
 * 				%[flag] [width].[precision] type
 *
 *			flags:
 *				+   - print sign
 *				' ' - padd using spaces - default
 *				0	- padd left with zeros
 *				-   - left align the result	
 *              *   - read width as next char, printf( "%*.2f", char_width, float_value); 
 *
 *			width:
 *				The minimual width the parameter should expand to. May excced but never less.
 *
 *			Precision:
 *				floating point - number of digits left of the decimal point
 *				types x,X,d    - No effect
 *				strings    	   - Maximum number chars read from pointer	parameter
 *
 *			types:
 *				x 	- lower case hex
 *				X 	- upper case hex
 *              o   - octal
 *              d 	- signed decimal value
 *              s 	- char * string 
 *              e   - floating point in expodential format
 *				E   - floating point in expodential format
 *             	f   - floating point 
 *
 *			
 *
 *  RETURNS: # chars printed
 *
 *
 *******************************************************************/
int sprintf(CHAR *str, void *fmt, ...)
{
	//static CHAR str2[MAX_STRING];
	va_list ap;

	va_start(ap,fmt);

	
/*	if((UDWORD)fmt>=DATA_MEM_LIMIT)			//check to see if the pointer is to data or pgm memory
	{ 	//we have program memory
		strcpypgm2ram(str2,(const rom char *)fmt);
		return sprintf_va(str,str2,ap);
	}else*/
	{
		return sprintf_va(str,(char *)fmt,ap);
	}

	//return sprintf_va(str,(CHAR *)fmt,ap);
		
}
	
/*******************************************************************
 *  FUNCTION: printf
 *  AUTHOR 		= 	TRAMPAS STERN
 *  FILE 		=	strio.c
 *  DATE		=   2/6/2003  5:33:21 PM
 *
 *  PARAMETERS: 
 *
 *  DESCRIPTION: impelments sprintf
 * 				%[flag] [width].[precision] type
 *
 *			flags:
 *				+   - print sign
 *				' ' - padd using spaces - default
 *				0	- padd left with zeros
 *				-   - left align the result	
 *              *   - read width as next char, printf( "%*.2f", char_width, float_value); 
 *
 *			width:
 *				The minimual width the parameter should expand to. May excced but never less.
 *
 *			Precision:
 *				floating point - number of digits left of the decimal point
 *				types x,X,d    - No effect
 *				strings    	   - Maximum number chars read from pointer	parameter
 *
 *			types:
 *				x 	- lower case hex
 *				X 	- upper case hex
 *              o   - octal
 *              d 	- signed decimal value
 *              s 	- char * string 
 *              e   - floating point in expodential format
 *				E   - floating point in expodential format
 *             	f   - floating point 
 *
 *			
 *
 *  RETURNS: # chars printed
 *
 *
 *******************************************************************/
int printf(void *fmt, ...)
{
	int ret=0;
	static char str[MAX_STRING]={0};
	//static char str2[MAX_STRING]={0};
	CHAR *p;
 
 	va_list ap;

	va_start(ap,fmt);

/*	if((UDWORD)fmt>=DATA_MEM_LIMIT)			//check to see if the pointer is to data or pgm memory
	{ 	//we have program memory
		strcpypgm2ram(str2,(const rom char *)fmt);
		sprintf_va(str,str2,ap);
	}else*/
	{
		ret=sprintf_va(str,(char *)fmt,ap);
	}


	p=str;
	if (strlen(p)>MAX_STRING)
	{
		str[20]=0;
		//log_printf("ERROR: printf with large str %s",str);
		HALT();
	}
	while(*p)
	{
		putch(*p);
		p++;
	}

	return ret;
}
	 
		
/******************************************************************* 
 *  FUNCTION :puts
 *
 *  AUTHOR: Trampas Stern 
 *
 *  DATE : 5/14/2003	8:22:59 AM
 *
 *  FILENAME: strio.c
 *
 *  PARAMETERS:
 *
 *  DESCRIPTION: 
 *		Prints a string to out
 *
 *  RETURNS:
 *
 *******************************************************************/
int puts(CHAR *str)
{
	int i=0;

	while(*str)
	{
		putch(*str++);
		i++;
	}
	return i;
}


/******************************************************************* 
 *  FUNCTION :putsf
 *
 *  AUTHOR: Trampas Stern 
 *
 *  DATE : 5/14/2003	8:24:38 AM
 *
 *  FILENAME: strio.c
 *
 *  PARAMETERS:
 *
 *  DESCRIPTION: 
 *		Prints a float
 *
 *  RETURNS:
 *
 *******************************************************************/
int putsf(float x, char precision)
{
	char str[MAX_STRING];

	ftoa(x,str,precision,'f');
	return puts(str);
}
	
/******************************************************************* 
 *  FUNCTION :putsx
 *
 *  AUTHOR: Trampas Stern 
 *
 *  DATE : 2/19/2003	10:55:34 AM
 *
 *  FILENAME: strio.c
 *
 *  PARAMETERS:
 *
 *  DESCRIPTION:
 *
 *  RETURNS:
 *
*******************************************************************/
int putsx(long data)
{
	char str[MAX_STRING];
	
	hextoa(str,data,0);
	return puts(str);
}

/******************************************************************* 
 *  FUNCTION :putsd
 *
 *  AUTHOR: Trampas Stern 
 *
 *  DATE : 2/19/2003	10:56:24 AM
 *
 *  FILENAME: strio.c
 *
 *  PARAMETERS:
 *
 *  DESCRIPTION:
 *
 *  RETURNS:
 *
*******************************************************************/
int putsd(long data)
{
	char str[MAX_STRING];

	tbs_ltoa(str,data,1);
	return puts(str);
}

	
	 
/******************************************************************* 
 *  FUNCTION :strcpy
 *
 *  AUTHOR: Trampas Stern 
 *
 *  DATE : 3/19/2003	11:02:34 AM
 *
 *  FILENAME: strio.c
 *
 *  PARAMETERS:
 *
 *  DESCRIPTION: 
 *		Copies a string
 *
 *  RETURNS:	 num chars
 *
 *******************************************************************/

int tbsstrcpy(const CHAR *src, char *dst)
{
	unsigned char i;
	const CHAR *ptr;
	i=0;
	ptr=(const CHAR *)src;
	while(*ptr)
	{
	   *dst++=*ptr++;
		i++;
	}
	*dst++=0;
	return i;
}
	

/******************************************************************* 
 *  FUNCTION :strlen
 *
 *  AUTHOR: Trampas Stern 
 *
 *  DATE : 3/20/2003	1:57:53 PM
 *
 *  FILENAME: strio.c
 *
 *  PARAMETERS:
 *
 *  DESCRIPTION: 
 *		returns the length of a string
 *
 *  RETURNS:
 *
 *******************************************************************/
 /*
int strlen(const CHAR *str)
{
	
	unsigned char i;
	const CHAR *ptr;
	i=0;
	ptr=(const CHAR *)str;
	while(*ptr++)
	{
		i++;
	}
	return i;
}
*/




