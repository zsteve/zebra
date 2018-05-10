#ifndef ZMEMORY_H
#define ZMEMORY_H

#include "../zglobal/zglobal.h"
#include "../zerror/zerror.h"
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

struct ZHeaderData{
	ZHeaderData(){

	}

	enum InterpreterNumber{
		DECSYSTEM_20,
		APPLE_IIe,
		MACINTOSH,
		AMIGA,
		ATARI_ST,
		IBM_PC,
		COMMODORE_128,
		COMMODORE_64,
		APPLE_IIc,
		APPLE_IIgs,
		TANDY_COLOR
	};

	bool status_line_not_available;
	bool screen_splitting_available;
	bool variable_pitch_font_default;
	bool colours_available;
	bool picture_disp_available;
	bool boldface_available;
	bool italic_available;
	bool fixed_space_font_available;
	bool sound_effects_available;
	bool timed_kb_input_available;
	bool transcripting_on;
	bool force_printing_fixed_pitch_font;
	bool game_wants_pictures;
	bool game_wants_undo;
	bool game_wants_mouse;
	bool game_wants_colours;
	bool game_wants_sound_effects;
	bool game_wants_menus;
	InterpreterNumber interpreter_number;
	zbyte interpreter_version;

	zbyte screen_height;
	zbyte screen_width;
	zbyte screen_width_units;
	zbyte screen_height_units;
	zbyte font_width_units;
	zbyte font_height_units;
	zbyte default_background_color;
	zbyte default_foreground_color;
	zbyte standard_revision_number;
};

class ZMemory{
    private:
    // raw memory data members
    zchar* zMemPtr;
	zchar* zMemOrig;			/*	"original" zMemOrig array holding original data
									(used for restarting the game)
								*/
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

	void writeHeaderData(ZHeaderData& headerData);

	void restoreOriginalMemory();

    ZMemory();
    ZMemory(zbyte* zData, ulong zDataLength);
    ~ZMemory();
    protected:
};

#endif
