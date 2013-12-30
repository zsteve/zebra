/************************************************
 num2ascii library : a library for converting
 integers to ASCII representations and vice versa
 
 code by Stephen Zhang (zsteve) 2013
*************************************************/

#include <cstdlib>
#include <conio.h>
#include <cstdio>
#include <iostream>

using namespace std;

#include "num2ascii.h"

char input[80];

void main()
{
	printf("num2ascii - C number to ascii conversion library\n");

	// unit test 

	printf("%s\n", IntegerToDecASCII(0xFFFF));
	printf("%s\n", IntegerToHexASCII(65535));
	printf("%s\n", IntegerToBinASCII(255));
	printf("%s\n", IntegerToOctASCII(4095));

	printf("decimal input >");
	scanf("%s", input);
	printf("%d", DecASCIIToInteger(FixASCIIPrefix(input, PREFIX_DEC_0D)));

	printf("\n");
	printf("hexadecimal input >");
	scanf("%s", input);
	printf("%d", HexASCIIToInteger(FixASCIIPrefix(input, PREFIX_HEX_0X)));

	printf("\n");
	printf("binary input >");
	scanf("%s", input);
	printf("%d", BinASCIIToInteger(FixASCIIPrefix(input, PREFIX_BIN_0B)));

	printf("\n");
	printf("octal input >");
	scanf("%s", input);
	printf("%d", OctASCIIToInteger(FixASCIIPrefix(input, PREFIX_DEC_0D)));
	getch();
}