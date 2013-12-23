#ifndef ZMEMORY_H
#define ZMEMORY_H

#include "../../zglobal/zglobal.h"
#include "../../zerror/zerror/zerror.h"
#include <exception>

#define ZMEMORY_DYNAMIC 0
#define ZMEMORY_STATIC 1
#define ZMEMORY_HIGH 2

#define THROW_ZMEMORYWRITEOUTOFBOUNDS(line, function, file)\
    throw ZMemoryWriteOutOfBounds((const int)line, (const char*)function, (const char*)file);

#define THROW_ZMEMORYREADOUTOFBOUNDS(line, function, file)\
    throw ZMemoryReadOutOfBounds((const int)line, (const char*)function, (const char*)file);

extern int zVersion;
extern ZError zErrorLogger;

class ZMemoryWriteOutOfBounds : ZException{
    public:
    ZMemoryWriteOutOfBounds(){
        zErrorLogger.addError("Error : ZMemoryWriteOutOfBounds thrown");
    }
    ZMemoryWriteOutOfBounds(const int line, const char* function, const char* file){
        zErrorLogger.addError(("Error : ZMemoryWriteOutOfBounds thrown at : "+compileErrorMsg(line, function, file)).c_str());
    }
};

class ZMemoryReadOutOfBounds : ZException{
    public:
    ZMemoryReadOutOfBounds(){
        zErrorLogger.addError("Error : ZMemoryReadOutOfBounds thrown");
    }
    ZMemoryReadOutOfBounds(const int line, const char* function, const char* file){
        zErrorLogger.addError(("Error : ZMemoryReadOutOfBounds thrown at : "+compileErrorMsg(line, function, file)).c_str());
    }
};

class ZMemory{
    private:
    // raw memory data members
    zchar* zMemPtr;
    ulong zMemSize;
    // memory region members
    // these ints are the addresses of the lower and upper bounds
    // of the z-machine's memory
    ulong zDynamicMemoryLower, zDynamicMemoryUpper;
    ulong zStaticMemoryLower, zStaticMemoryUpper;
    ulong zHighMemoryLower, zHighMemoryUpper;
    bool addrIsWithinBounds(ulong addr, int whichMemory);
    ulong zGlobalVarsAddr;

    public:

    // accessor functions
    zbyte* getRawDataPtr(){return zMemPtr;}
    ulong getMemSize(){return zMemSize;}

    ulong getZDynamicMemoryLower(){return zDynamicMemoryLower;}
    ulong getZDynamicMemoryUpper(){return zDynamicMemoryUpper;}
    ulong getZStaticMemoryLower(){return zStaticMemoryLower;}
    ulong getZStaticMemoryUpper(){return zStaticMemoryUpper;}
    ulong getZHighMemoryLower(){return zHighMemoryLower;}
    ulong getZHighMemoryUpper(){return zHighMemoryUpper;}
	ulong getGlobalVarsAddr(){return zGlobalVarsAddr;}

    // memory read functions

    zword readZWord(ulong addr) throw (ZMemoryReadOutOfBounds);
    zword readZWordPackedAddr(zword addr) throw (ZMemoryReadOutOfBounds);
    zbyte readZByte(ulong addr) throw (ZMemoryReadOutOfBounds);
    zbyte readZBytePackedAddr(zword addr) throw (ZMemoryReadOutOfBounds);

    void storeZWord(ulong addr, zword data) throw (ZMemoryWriteOutOfBounds);
    void storeZWordPackedAddr(zword addr, zword data) throw (ZMemoryWriteOutOfBounds);
    void storeZByte(ulong addr, zbyte data) throw (ZMemoryWriteOutOfBounds);
    void storeZBytePackedAddr(zword addr, zbyte data) throw (ZMemoryWriteOutOfBounds);

	ulong unpackAddr(zword addr);

	zword readGlobalVar(zbyte varNum);
	void storeGlobalVar(zbyte varNum, zword newVal);

    ZMemory();

    ZMemory(zbyte* zData, ulong zDataLength);

    ~ZMemory();
    protected:
};

#endif
