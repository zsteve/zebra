#include "zglobal.h"

zword endianize(zword in)
{
    // converts a zword from big-endian to little-endian
    // and vice-versa
    zchar inHi, inLo;
    inHi=in>>8;
    inLo=(in&255);
    zword out=0;
    out|=inLo;
    out = out << 8;
    out|=inHi;
    return out;
}

zword reverseBitSequence(zbyte in)
{
    // reverses the bit sequence in a byte
    // i.e - 10010011 --> 11001001
    zbyte bits[8]={0, 0, 0, 0, 0, 0, 0, 0};
    bits[0]=(in&1);
    bits[1]=(in&2)>>1;
    bits[2]=(in&4)>>2;
    bits[3]=(in&8)>>3;
    bits[4]=(in&16)>>4;
    bits[5]=(in&32)>>5;
    bits[6]=(in&64)>>6;
    bits[8]=(in&128)>>7;
    zbyte outByte=0;
    outByte=(outByte|(bits[0]<<7));
    outByte=(outByte|(bits[1]<<6));
    outByte=(outByte|(bits[2]<<5));
    outByte=(outByte|(bits[3]<<4));
    outByte=(outByte|(bits[4]<<3));
    outByte=(outByte|(bits[5]<<2));
    outByte=(outByte|(bits[6]<<1));
    outByte=(outByte|(bits[7]));
    return outByte;
}
