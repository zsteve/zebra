/************************************************
 num2ascii library : a library for converting
 integers to ASCII representations and vice versa

 code by Stephen Zhang (zsteve) 2013
*************************************************/

#include "num2ascii.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char DecASCIINums[]={"0123456789"};
char HexASCIINums[]={"0123456789ABCDEF"};
char BinASCIINums[]={"01"};
char OctASCIINums[]={"01234567"};

int CmpChar(char a, char b)
{
	char a_, b_;
	a_= isupper(a) ? tolower(a) : a;
	b_= isupper(b) ? tolower(b) : b;
	if(a_!=b_) return 0;
	return 1;
}

/**
 * returns index of char c in CSTRING s
 * or RET_FAILURE if it is not present
 */
int GetCharIndex(char c, CSTRING s)
{
	int i=0;
	if(!s)
	{
		return RET_FAILURE;
	}
	do{
		if(CmpChar(s[i], c))
		{
			return i;
		}
		i++;
	}while(s[i]!=(char)NULL);
	return RET_FAILURE;
}

char GetCharAt(CSTRING s, int i)
{
	if(strlen(s)<i) return RET_FAILURE;
	return s[i];
}

/**
 * converts a decimal ASCII string to an integer
 * returns RET_FAILURE if the string contains non numeric chars
 */
int DecASCIIToInteger(CSTRING s)
{
	int out_int=0;
	int i=0;
	int neg=0;
	if(!s) return RET_FAILURE;
	if(s[0]=='-'){
		// is negative
		neg=1;
	}
	do{
		int char_int=GetCharIndex(s[i], DecASCIINums);
		if(char_int==RET_FAILURE)
		{
			return RET_FAILURE;	// invalid char (non numeric)
		}
		out_int+=char_int;
		out_int*=10;
		i++;
	}while(s[i]!=(char)NULL);
	out_int/=10;
	return neg ? -(out_int) : out_int;
}

/**
 * converts a hexadecimal ASCII string to an integer
 * returns RET_FAILURE if the string contains chars outside of the range [0, 1, 2, ... F]
 */
int HexASCIIToInteger(CSTRING s)
{
	int out_int=0;
	int i=0;
	if(!s) return RET_FAILURE;
	do{
		int char_int=GetCharIndex(s[i], HexASCIINums);
		if(char_int==RET_FAILURE)
		{
			return RET_FAILURE;	// invalid char (non numeric)
		}
		out_int|=char_int;
		out_int<<=4;
		i++;
	}while(s[i]!=(char)NULL);
	out_int>>=4;
	return out_int;
}

/**
 * converts a binary ASCII string to an integer
 * returns RET_FAILURE if the string contains chars outside of the range [0, 1]
 */
int BinASCIIToInteger(CSTRING s)
{
	int out_int=0;
	int i=0;
	if(!s) return RET_FAILURE;
	do{
		int char_int=GetCharIndex(s[i], BinASCIINums);
		if(char_int==RET_FAILURE)
		{
			return RET_FAILURE;	// invalid char (non numeric)
		}
		out_int|=char_int;
		out_int<<=1;
		i++;
	}while(s[i]!=(char)NULL);
	out_int>>=1;
	return out_int;
}

int OctASCIIToInteger(CSTRING s)
{
	int out_int=0;
	int i=0;
	if(!s) return RET_FAILURE;
	do{
		int char_int=GetCharIndex(s[i], OctASCIINums);
		if(char_int==RET_FAILURE)
		{
			return RET_FAILURE;	// invalid char (non numeric)
		}
		out_int|=char_int;
		out_int<<=3;
		i++;
	}while(s[i]!=(char)NULL);
	out_int>>=3;
	return out_int;
}

// out_str is the char array for all int to ASCII conversions
// is returned by every IntegerTo???ASCII() function
char out_str[sizeof(int)*8+1];	// max size of out_str is number of bits in an int

void SwapChar(CSTRING c1, CSTRING c2)
{
	char temp=*c1;
	*c1=*c2;
	*c2=temp;
	return;
}

void ReverseString(CSTRING s)
{
	// reverses the order of characters in a string
	int len=strlen(s);
	int i, j;
	if(len==1 || !len) return;
	for(i=0, j=len-1; i!=j && i<j; i++, j--)
	{
		SwapChar(&s[i], &s[j]);
	}
}

CSTRING IntegerToDecASCII(int n)
{
	int i=0;
	int z=0;
	int x=n;
	int neg=0;
	if(n<0){
		neg=1;
		out_str[0]='-';
		x=abs(n);
		i=1;
	}
	for(;z<sizeof(out_str); z++) out_str[z]=0;
	do{
		int m=(x%10);
		out_str[i]=GetCharAt(DecASCIINums, m);
		x=((x-(x%10))/10);
		i++;
	}while(x!=0);
	out_str[i]=0;
	ReverseString(out_str);
	return out_str;
}

CSTRING IntegerToHexASCII(int n)
{
	int i=0;
	int x=n;
	do{
		int m=(x%16);
		out_str[i]=GetCharAt(HexASCIINums, m);
		x>>=4;
		i++;
	}while(x!=0);
	out_str[i]=0;
	ReverseString(out_str);
	return out_str;
}

CSTRING IntegerToBinASCII(int n)
{
	int i=0;
	int x=n;
	do{
		int m=(x&1);
		out_str[i]=GetCharAt(BinASCIINums, m);
		x>>=1;
		i++;
	}while(x!=0);
	out_str[i]=0;
	ReverseString(out_str);
	return out_str;
}

CSTRING IntegerToOctASCII(int n)
{
	int i=0;
	int x=n;
	do{
		int m=(x&7);
		out_str[i]=GetCharAt(OctASCIINums, m);
		x>>=3;
		i++;
	}while(x!=0);
	out_str[i]=0;
	ReverseString(out_str);
	return out_str;
}

char out_str_1[sizeof(int)*8+1];	// char array used for FixASCIIPrefix()

void StrCpy_Safe(CSTRING dest, CSTRING src, int size)
{
	int i=0;
	for(i=0; i<size; i++){
		dest[i]=src[i];
		if(!src[i]) break;
	}
}

CSTRING FixASCIIPrefix(CSTRING s, int p)
{
	if(p==PREFIX_DEC_0D &&
		CmpChar(s[0], '0') &&
		CmpChar(s[1], 'd')){

		StrCpy_Safe(out_str_1, s+2, sizeof(out_str_1));
	}else if(p==PREFIX_HEX_0X &&
		CmpChar(s[0], '0') &&
		CmpChar(s[1], 'x')){

		StrCpy_Safe(out_str_1, s+2, sizeof(out_str_1));
	}else if(p==PREFIX_BIN_0B &&
		CmpChar(s[0], '0') &&
		CmpChar(s[1], 'b')){

		StrCpy_Safe(out_str_1, s+2, sizeof(out_str_1));
	}else{
		StrCpy_Safe(out_str_1, s, sizeof(out_str_1));
	}
	return out_str_1;
}
