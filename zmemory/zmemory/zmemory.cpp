#include "zmemory.h"
#include "../../zglobal/zglobal.h"

ZMemory::ZMemory()
{
    zMemPtr=NULL;
    zMemSize=0;
    zDynamicMemoryLower=zDynamicMemoryUpper=0;
    zStaticMemoryLower=zStaticMemoryUpper=0;
    zHighMemoryLower=zHighMemoryUpper=0;
}

ZMemory::ZMemory(zbyte* zData, ulong zDataLength)
{
    /*
    The memory map of the Z-machine is an array of bytes with "byte addresses" running from 0 upwards. This is divided ulongo three regions:
    "dynamic", "static" and "high". Dynamic memory begins from byte address $00000 and runs up to the byte before the byte address stored in
    the word at $0e in the header. (Dynamic memory must contain at least 64 bytes.) Static memory follows immediately on. Its extent is not
    defined in the header (or anywhere else), though it must end by the last byte of the story file or by byte address $0ffff (whichever is
    lower). High memory begins at the "high memory mark" (the byte address stored in the word at $04 in the header) and continues to the end
    of the story file. The bottom of high memory may overlap with the top of static memory (but not with dynamic memory).
    */

    zDynamicMemoryLower=0x0;
    zDynamicMemoryUpper=(ulong)(endianize(*((zword*)(zData+0xe))));
    zStaticMemoryLower=zDynamicMemoryUpper+1;
    zStaticMemoryUpper=(zDataLength<0xFFFF ? zDataLength : 0xFFFF);
    zHighMemoryLower=(ulong)endianize(*((zword*)(zData+0x4)));
    zHighMemoryUpper=zDataLength;
    zVersion=(int)zData[0];
    zMemPtr=new zbyte[zDataLength];
    for(ulong i=0; i<zDataLength; i++)
    {
        zMemPtr[i]=zData[i];
    }
}

ZMemory::~ZMemory()
{
    if(zMemPtr)
        delete[] zMemPtr;
}

/* games may write to any address within dynamic memory
   games may only read from addresses within static memory
   games may NOT directly access addresses within high memory */

bool ZMemory::addrIsWithinBounds(ulong addr, int whichMemory)
{
    switch(whichMemory){
        case ZMEMORY_DYNAMIC:
            if((addr>=zDynamicMemoryLower) && (addr<=zDynamicMemoryUpper))
            {
                return true;
            }else{
                return false;
            }
            break;  // added break anyway
        case ZMEMORY_STATIC:
            if((addr>=zStaticMemoryLower) && (addr<=zStaticMemoryUpper))
            {
                return true;
            }else{
                return false;
            }
            break;
        case ZMEMORY_HIGH:
            if((addr>=zHighMemoryLower) && (addr<=zHighMemoryUpper))
            {
                return true;
            }else{
                return false;
            }
        default:
        return false;
    }
}

zword ZMemory::readZWord(zword addr) throw (ZMemoryReadOutOfBounds)
{
    // Reads an endianized zword from addr
    if((addrIsWithinBounds(addr, ZMEMORY_DYNAMIC)) || addrIsWithinBounds(addr, ZMEMORY_STATIC))
    {
        return (zword)(endianize(*((zword*)(zMemPtr+addr))));
    }else{
        throw ZMemoryReadOutOfBounds();
    }
}

zword ZMemory::readZWordPackedAddr(zword addr) throw (ZMemoryReadOutOfBounds)
{
    ulong zLongAddr=0;
    if(zVersion<=3){
        zLongAddr=(addr<<1);
    }else if(zVersion==4 || zVersion==5){
        zLongAddr=(addr<<2);
    }else{
        throw ZMemoryReadOutOfBounds(); // unsupported version!
    }
    if(addrIsWithinBounds(zLongAddr, ZMEMORY_DYNAMIC) || addrIsWithinBounds(zLongAddr, ZMEMORY_STATIC)){
        return (zword)(endianize(*((zword*)(zMemPtr+zLongAddr))));
    }else{
        throw ZMemoryReadOutOfBounds();
    }
}

zbyte ZMemory::readZByte(zword addr) throw (ZMemoryReadOutOfBounds)
{
    if((addrIsWithinBounds(addr, ZMEMORY_DYNAMIC)) || addrIsWithinBounds(addr, ZMEMORY_STATIC))
    {
        return (zbyte)*(zMemPtr+addr);
    }else{
        throw ZMemoryReadOutOfBounds();
    }
}

zbyte ZMemory::readZBytePackedAddr(zword addr) throw (ZMemoryReadOutOfBounds)
{
    ulong zLongAddr=0;
    if(zVersion<=3){
        zLongAddr=(addr<<1);
    }else if(zVersion==4 || zVersion==5){
        zLongAddr=(addr<<2);
    }else{
        throw ZMemoryReadOutOfBounds(); // unsupported version!
    }
    if(addrIsWithinBounds(zLongAddr, ZMEMORY_DYNAMIC) || addrIsWithinBounds(zLongAddr, ZMEMORY_STATIC)){
        return (zbyte)*(zMemPtr+addr);
    }else{
        throw ZMemoryReadOutOfBounds();
    }
}

void ZMemory::storeZWord(zword addr, zword data) throw (ZMemoryWriteOutOfBounds)
{
    if(addrIsWithinBounds(addr, ZMEMORY_DYNAMIC)){
        *((zword*)(zMemPtr+addr))=endianize(data);
        return;
    }else{
        throw ZMemoryWriteOutOfBounds();
    }
}

void ZMemory::storeZWordPackedAddr(zword addr, zword data) throw (ZMemoryWriteOutOfBounds)
{
    ulong zLongAddr=0;
    if(zVersion<=3){
        zLongAddr=(addr<<1);
    }else if(zVersion==4 || zVersion==5){
        zLongAddr=(addr<<2);
    }else{
        throw ZMemoryReadOutOfBounds(); // unsupported version!
    }
    if(addrIsWithinBounds(zLongAddr, ZMEMORY_DYNAMIC)){
        *((zword*)(zMemPtr+zLongAddr))=endianize(data);
        return;
    }else{
        throw ZMemoryReadOutOfBounds();
    }
}

void ZMemory::storeZByte(zword addr, zbyte data) throw (ZMemoryWriteOutOfBounds)
{
    if(addrIsWithinBounds(addr, ZMEMORY_DYNAMIC)){
        *(zMemPtr+addr)=data;
        return;
    }else{
        throw ZMemoryWriteOutOfBounds();
    }
}

void ZMemory::storeZBytePackedAddr(zword addr, zbyte data) throw (ZMemoryWriteOutOfBounds)
{
    ulong zLongAddr=0;
    if(zVersion<=3){
        zLongAddr=(addr<<1);
    }else if(zVersion==4 || zVersion==5){
        zLongAddr=(addr<<2);
    }else{
        throw ZMemoryReadOutOfBounds(); // unsupported version!
    }
    if(addrIsWithinBounds(zLongAddr, ZMEMORY_DYNAMIC)){
        *(zMemPtr+zLongAddr)=data;
        return;
    }else{
        throw ZMemoryReadOutOfBounds();
    }
}
