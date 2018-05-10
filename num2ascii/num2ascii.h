/************************************************
 num2ascii library : a library for converting
 integers to ASCII representations and vice versa

 code by Stephen Zhang (zsteve) 2013
*************************************************/
#ifndef NUM2ASCII_H
#define NUM2ASCII_H

#ifdef __cplusplus
extern "C" {
#endif

typedef char* CSTRING;

#define RET_FAILURE -1

#define PREFIX_DEC_0D 1		// "0d..." prefix for dec
#define PREFIX_HEX_0X 2		// "0x..." prefix for hex
#define PREFIX_BIN_0B 3		// "0b..." prefix for bin

// function declarations

// ASCII to integer
int DecASCIIToInteger(CSTRING s);
int HexASCIIToInteger(CSTRING s);
int BinASCIIToInteger(CSTRING s);
int OctASCIIToInteger(CSTRING s);

// Integer to ASCII
CSTRING IntegerToDecASCII(int n);
CSTRING IntegerToHexASCII(int n);
CSTRING IntegerToBinASCII(int n);
CSTRING IntegerToOctASCII(int n);

// Prefix fixers
CSTRING FixASCIIPrefix(CSTRING s, int p);

#ifdef __cplusplus
}
#endif

#endif
