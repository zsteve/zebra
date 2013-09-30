#ifndef ZGLOBAL_H
#define ZGLOBAL_H
#include "../zerror/zerror/zerror.h"

// typedefs
typedef unsigned short zword;
typedef unsigned char zchar;
typedef unsigned char zbyte;
typedef unsigned long ulong;

// functions

zword endianize(zword in);
zword reverseBitSequence(zbyte in);

#endif
