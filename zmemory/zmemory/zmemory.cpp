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
    zGlobalVarsAddr=(ulong)(endianize(*((zword*)(zData+0xc))));
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

zword ZMemory::readZWord(ulong addr) throw (ZMemoryReadOutOfBounds)
{
    // Reads an endianized zword from addr
    if((addrIsWithinBounds(addr, ZMEMORY_DYNAMIC))
        || addrIsWithinBounds(addr, ZMEMORY_STATIC)
        || addrIsWithinBounds(addr, ZMEMORY_HIGH))
    {
        return (zword)(endianize(*((zword*)(zMemPtr+addr))));
    }else{
        THROW_ZMEMORYREADOUTOFBOUNDS(__LINE__, __FUNCTION__, __FILE__);
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
        THROW_ZMEMORYREADOUTOFBOUNDS(__LINE__, __FUNCTION__, __FILE__); // unsupported version!
    }
    if((addrIsWithinBounds(addr, ZMEMORY_DYNAMIC))
        || addrIsWithinBounds(addr, ZMEMORY_STATIC)
        || addrIsWithinBounds(addr, ZMEMORY_HIGH)){
        return (zword)(endianize(*((zword*)(zMemPtr+zLongAddr))));
    }else{
        THROW_ZMEMORYREADOUTOFBOUNDS(__LINE__, __FUNCTION__, __FILE__);
    }
}

zbyte ZMemory::readZByte(ulong addr) throw (ZMemoryReadOutOfBounds)
{
    if((addrIsWithinBounds(addr, ZMEMORY_DYNAMIC))
        || addrIsWithinBounds(addr, ZMEMORY_STATIC)
        || addrIsWithinBounds(addr, ZMEMORY_HIGH))
    {
        return (zbyte)*(zMemPtr+addr);
    }else{
        THROW_ZMEMORYREADOUTOFBOUNDS(__LINE__, __FUNCTION__, __FILE__);
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
        THROW_ZMEMORYREADOUTOFBOUNDS(__LINE__, __FUNCTION__, __FILE__); // unsupported version!
    }
    if((addrIsWithinBounds(addr, ZMEMORY_DYNAMIC))
        || addrIsWithinBounds(addr, ZMEMORY_STATIC)
        || addrIsWithinBounds(addr, ZMEMORY_HIGH)){
        return (zbyte)*(zMemPtr+addr);
    }else{
        THROW_ZMEMORYREADOUTOFBOUNDS(__LINE__, __FUNCTION__, __FILE__);
    }
}

void ZMemory::storeZWord(ulong addr, zword data) throw (ZMemoryWriteOutOfBounds)
{
    if(addrIsWithinBounds(addr, ZMEMORY_DYNAMIC)){
        *((zword*)(zMemPtr+addr))=endianize(data);
        return;
    }else{
        THROW_ZMEMORYWRITEOUTOFBOUNDS(__LINE__, __FUNCTION__, __FILE__);
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
        THROW_ZMEMORYREADOUTOFBOUNDS(__LINE__, __FUNCTION__, __FILE__); // unsupported version!
    }
    if(addrIsWithinBounds(zLongAddr, ZMEMORY_DYNAMIC)){
        *((zword*)(zMemPtr+zLongAddr))=endianize(data);
        return;
    }else{
        THROW_ZMEMORYREADOUTOFBOUNDS(__LINE__, __FUNCTION__, __FILE__);
    }
}

void ZMemory::storeZByte(ulong addr, zbyte data) throw (ZMemoryWriteOutOfBounds)
{
    if(addrIsWithinBounds(addr, ZMEMORY_DYNAMIC)){
        *(zMemPtr+addr)=data;
        return;
    }else{
        THROW_ZMEMORYWRITEOUTOFBOUNDS(__LINE__, __FUNCTION__, __FILE__);
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
        THROW_ZMEMORYREADOUTOFBOUNDS(__LINE__, __FUNCTION__, __FILE__); // unsupported version!
    }
    if(addrIsWithinBounds(zLongAddr, ZMEMORY_DYNAMIC)){
        *(zMemPtr+zLongAddr)=data;
        return;
    }else{
        THROW_ZMEMORYREADOUTOFBOUNDS(__LINE__, __FUNCTION__, __FILE__);
    }
}

ulong ZMemory::unpackAddr(zword addr){
    ulong zLongAddr=0;
    if(zVersion<=3){
        zLongAddr=(addr<<1);
    }else if(zVersion==4 || zVersion==5){
        zLongAddr=(addr<<2);
    }
	return zLongAddr;
}

void ZMemory::writeHeaderData(ZHeaderData& header){
	// write header data to story memory
	if(zVersion<=3){
		// versions 1-3 : flags 1 are different
		zbyte flags1=readZByte(0x1);
		flags1|=(header.status_line_not_available)<<4;
		flags1|=(header.screen_splitting_available)<<5;
		flags1|=(header.variable_pitch_font_default)<<6;
		storeZByte(0x1, flags1);
	}else{
		zbyte flags1=readZByte(0x1);
		flags1|=(header.colours_available);
		flags1|=(header.picture_disp_available)<<1;
		flags1|=(header.boldface_available)<<2;
		flags1|=(header.italic_available)<<3;
		flags1|=(header.fixed_space_font_available)<<4;
		flags1|=(header.sound_effects_available)<<5;
		flags1|=(header.timed_kb_input_available)<<7;
		storeZByte(0x1, flags1);
	}
	// flags 2
	zword flags2=readZWord(0x10);
	flags2|=(header.transcripting_on);
	if(zVersion>=3){
		flags2|=(header.force_printing_fixed_pitch_font)<<1;
	}if(zVersion>=5){
		flags2|=header.game_wants_pictures<<3;
		flags2|=header.game_wants_undo<<4;
		flags2|=header.game_wants_mouse<<5;
		flags2|=header.game_wants_sound_effects<<7;
	}if(zVersion>=6){
		flags2|=header.game_wants_menus<<8;
	}
	storeZWord(0x10, flags2);
	// int num
	storeZByte(0x1e, header.interpreter_number);
	storeZByte(0x1f, header.interpreter_version);
	// more values
	if(zVersion>=4){
		storeZByte(0x20, header.screen_height);
		storeZByte(0x21, header.screen_width);
	}if(zVersion>=5){
		storeZWord(0x22, header.screen_width_units);
		storeZWord(0x24, header.screen_height_units);
		storeZByte(0x26, header.font_width_units);
		storeZByte(0x27, header.font_height_units);
		storeZByte(0x2c, header.default_background_color);
		storeZByte(0x2d, header.default_foreground_color);
	}
	storeZWord(0x32, header.standard_revision_number);
}

zword ZMemory::readGlobalVar(zbyte varNum){
	// global vars are 0x10 - 0xff
	if((varNum<0x10) || (varNum>0xff)){
		throw ZMemoryReadOutOfBounds(__LINE__, __FUNCTION__, __FILE__);
	}
	return readZWord(getGlobalVarsAddr()+((varNum-0x10)*2));
}

void ZMemory::storeGlobalVar(zbyte varNum, zword newVal){
	if((varNum<0x10) || (varNum>0xff)){
		throw ZMemoryWriteOutOfBounds(__LINE__, __FUNCTION__, __FILE__);
	}
	storeZWord(getGlobalVarsAddr()+((varNum-0x10)*2), newVal);
	return;
}