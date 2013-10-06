/** ztext.h
 * z-machine text functions
 **/

#ifndef ZTEXT_H
#define ZTEXT_H

#include <exception>
#include "../../zglobal/zglobal.h"
#include "../../zerror/zerror/zerror.h"

extern ZError zErrorLogger;

class IllegalZCharString : std::exception{
    public:
    IllegalZCharString(){
        zErrorLogger.addError("Error : IllegalZCharString thrown");
    }
};

class IllegalZCharException : std::exception{
    public:
    IllegalZCharException(){
        zErrorLogger.addError("Error : IllegalZCharException thrown");
    }
};

int zCharStrLen(zword* str);
int ZSCIIStrLen(zchar* str);
void ZSCIIStrCat(zchar* src, zchar* cat);
int ZSCIIGetResidentAlphabet(zchar zch) throw (IllegalZCharException);
zchar getZCharAlphaShiftCharacter(int currentAlpha, int desiredAlpha, bool shiftLock=false);

zchar* zCharStringtoZSCII(zword* zCharString);
zchar* zChartoZSCII(zword zChar, bool resetShifts=false);
zword ZSCIItoZChar(zchar* zscii, int& bytesConverted);
zword* ZSCIItoZCharString(zchar* zscii);

#endif
