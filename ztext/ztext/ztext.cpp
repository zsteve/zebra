#include <iostream>

using namespace std;

#include "ztext.h"
#include "../../zmemory/zmemory/zmemory.h"
#include <vector>

#define ALPHABET_1 1
#define ALPHABET_2 2
#define ALPHABET_0 0

char zAlphaTable0[]={"abcdefghijklmnopqrstuvwxyz"};
char zAlphaTable1[]={"ABCDEFGHIJKLMNOPQRSTUVWXYZ"};
char zAlphaTable2[]={" \n0123456789.,!?_#'\"/\s\-:()"};

char ZSCIIAlphaTable[]={" !\"#$%&'()*+,-./0123456789:;<=>?"\
						"@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_"\
						"'abcdefghijklmnopqrstuvwxyz{!}~ "};

extern int zVersion;

int defaultAlpha=ALPHABET_0;

int ZSCIIStrLen(zchar* str)
{
    // returns string length of ZSCII string (8-bit)
    int i=0;
    for(i=0; str[i]!=NULL; i++);
    return i;
}

int zCharStrLen(zword* str)
{
    // returns string length of ZCHAR string (5-bit)
    int i,j;
    i=0;
    j=3;
    for(;;i++, j+=3)
    {
        if((str[i])>>15)
        {
            break;
        }
    }
    return j;
}

void ZSCIIStrCat(zchar* src, zchar* cat)
{
    // concatacenates two ZSCII strings
    int i;
    for(i=0; src[i]!=0; i++);
    for(int j=0; cat[j]!=0; j++)
    {
        src[i++]=cat[j];
    }
    src[i]=NULL;
}

void ZSCIIStrCpy(zchar* src, zchar* dest)
{
	for(int i=0; src[i]!=NULL; i++)
	{
		dest[i]=src[i];
	}
}

std::vector<zchar> expandZChars(zword* zCharString)
{
    // expects little endian (x86) zwords
    std::vector<zchar> charVector(0);
    for(int i=0; ; i++)
    {
        zchar zUpper, zLower;
        zUpper=(zCharString[i]>>8);
        zLower=(zCharString[i]&255);
        charVector.push_back((zUpper>>2) & 31);
        charVector.push_back(((zUpper&3)<<3)|((zLower>>5)));
        charVector.push_back((zLower&31));
        if(zCharString[i]>>15) break;
    }
    return charVector;
}

zchar* zCharStringtoZSCII(zword* zCharString, ZMemory &zMem)
{
    // converts an entire z-char string
    // to a ZSCII string
    std::vector<zchar> charVector=expandZChars(zCharString);
    zCharStringtoZSCIIHelper(NULL, NULL, zMem, true);       // call zCharStringtoZSCIIHelper() once with resetShifts
                                                            // true so we can reset everything
    zchar* charArray=new zchar[charVector.size()];
    for(int i=0; i<charVector.size(); i++) charArray[i]=charVector[i];
    zchar* outData=zCharStringtoZSCIIHelper(charArray, charVector.size(), zMem);
    delete[] charArray;
    return outData;
}

zchar* zCharStringtoZSCII(ulong addr, ZMemory& zMem)
{
    vector<zword> zCharVect(0);
    for(int i=0; ; i+=2)
    {
        zCharVect.push_back(zMem.readZWord(addr+i));
        if(zCharVect[i>>1]>>15)
            break;
    }
    zword* zCharArr=new zword[zCharVect.size()];
    for(int i=0; i<zCharVect.size(); i++)
    {
        zCharArr[i]=zCharVect[i];
    }
    zchar* outData=zCharStringtoZSCII(zCharArr, zMem);
    delete[] zCharArr;
    return outData;
}

zword lookupAbbreviationAddr(ZMemory &zMem, zbyte zControlChar, zbyte nextChar) throw (ZMemoryReadOutOfBounds)
{
    // returns address of abbreviation string
    try{
        zword abbrevTableAddr=zMem.readZWord(0x18);
        zword entryOffset=(32*(zControlChar-1)+nextChar);
        zword outAddr=zMem.readZWord(abbrevTableAddr+entryOffset*2);
        return outAddr*2; // outAddr is a word address
    }catch(ZMemoryReadOutOfBounds e){
        throw e;
    }
}

struct zCharStringtoZSCIIHelper_saveState{
    int alphaShift;
    bool shiftFlag;
    zCharStringtoZSCIIHelper_saveState(int alphaShift, bool shiftFlag)
    {
        this->alphaShift=alphaShift;
        this->shiftFlag=shiftFlag;
    }
};

zchar* zCharStringtoZSCIIHelper(zchar* zCharString, ulong zStringLength, ZMemory& zMem, bool resetShifts)
{
    // converts a z-character string to
    // a string of ZSCII characters (in 16-bit zwords)
    // terminated by NULL
    // it is the caller's responsibility to free the
    // resulting array
    static int alphaShift;
    static bool shiftFlag;
    if(resetShifts)
    {
        alphaShift=0;
        shiftFlag=0;
        return NULL;
    }
    // now we map the z-characters to the ZSCII equivalents
    std::vector<zchar> zsciiString(0);
    for(int j=0, i=0; i<zStringLength; i++)
    {
        char xlatChar=zCharString[i];
        if(xlatChar==NULL)
        {
            // NULL == space
            zsciiString.push_back(' ');
            shiftFlag=false;
            alphaShift=ALPHABET_0;
            continue;
        }
		// special check for a ten bit ZSCII char
		if(alphaShift==ALPHABET_2 && xlatChar==6){
			//the two subsequent Z-characters specify a ten-bit ZSCII character code
			//the next Z-character gives the top 5 bits and the one after the bottom 5.
			char c1, c2;
			c1=zCharString[i+1];
			c2=zCharString[i+2];
			zword zscii=(c1<<5)|c2;
			zsciiString.push_back(ZSCIIAlphaTable[zscii-0x20]);
			i+=2;
		}

        // now we have to check for special chars
        // according to ZSpec, Z-chars 1, 2, 3, 4, 5
        // are special

        if(zVersion<3)
        // for versions lower than 3
        // z-char 1 - abbreviations (version 2 only!!)
        // z-chars 2..5 - alphabet shifting
        {
            if(xlatChar==2)
            {
                // if z-char == 4, shift next char
                if(alphaShift==ALPHABET_0)
                    alphaShift=ALPHABET_1;
                else if(alphaShift==ALPHABET_1)
                    alphaShift=ALPHABET_2;
                else if(alphaShift==ALPHABET_2)
                    alphaShift=ALPHABET_0;
                shiftFlag=true;
                continue;
            }else if(xlatChar==3){
                // if z-char == 5, shift next char to A2
                if(alphaShift==ALPHABET_0)
                    alphaShift=ALPHABET_2;
                else if(alphaShift==ALPHABET_1)
                    alphaShift=ALPHABET_0;
                else if(alphaShift==ALPHABET_2)
                    alphaShift=ALPHABET_1;
                shiftFlag=true;
                continue;
            }else if(xlatChar==4){
                if(defaultAlpha==ALPHABET_0)
                    defaultAlpha=ALPHABET_1;
                else if(defaultAlpha==ALPHABET_1)
                    defaultAlpha=ALPHABET_2;
                else if(defaultAlpha==ALPHABET_2)
                    defaultAlpha=ALPHABET_0;
                continue;
            }else if(xlatChar==5){
                if(defaultAlpha==ALPHABET_0)
                    defaultAlpha=ALPHABET_2;
                else if(defaultAlpha==ALPHABET_1)
                    defaultAlpha=ALPHABET_0;
                else if(defaultAlpha==ALPHABET_2)
                    defaultAlpha=ALPHABET_1;
                continue;
            }else if(xlatChar==1 && zVersion==2){
                // abbreviations for z-version 2
            }
        }else if(zVersion>=3){
            // for versions 3 and above
            // zchars 1..3 - abbrevations
            // zchars 4, 5 - alphabet shifting
            if(xlatChar==4){
                // No shift lock for versions 3 and up
                shiftFlag=true;
                if(alphaShift==ALPHABET_0)
                    alphaShift=ALPHABET_1;
                else if (alphaShift==ALPHABET_1)
                    alphaShift=ALPHABET_2;
                else if (alphaShift==ALPHABET_2)
                    alphaShift=ALPHABET_0;
                continue;
            }else if(xlatChar==5){
                shiftFlag=true;
                if(alphaShift==ALPHABET_0)
                    alphaShift=ALPHABET_2;
                else if (alphaShift==ALPHABET_1)
                    alphaShift=ALPHABET_0;
                else if (alphaShift==ALPHABET_2)
                    alphaShift=ALPHABET_1;
                continue;
            /**** TODO ****/
            // add support for abbreviations
            }else if(xlatChar==1){
                zCharStringtoZSCIIHelper_saveState temp(alphaShift, shiftFlag); // this is necessary to preserve the state of this function's
                                                                                // static members
                if((i+1)<zStringLength)	// this is to make sure we don't overstep array bounds when there is an incomplete mulit-zchar construct
                {
              	  zchar* abbrevStr=\
	                zCharStringtoZSCII(((ulong)(lookupAbbreviationAddr(zMem, xlatChar, zCharString[i+1]))), zMem);
	                for(int i=0; abbrevStr[i]!=0; i++)
	                    zsciiString.push_back(abbrevStr[i]);
	                delete[] abbrevStr;
	                i++;
	                alphaShift=temp.alphaShift;
	                shiftFlag=temp.shiftFlag;
	              }
                continue;
            }else if(xlatChar==2){
                zCharStringtoZSCIIHelper_saveState temp(alphaShift, shiftFlag); // this is necessary to preserve the state of this function's
                                                                                // static members
                if((i+1)<zStringLength)	// this is to make sure we don't overstep array bounds when there is an incomplete mulit-zchar construct
                {
              	  zchar* abbrevStr=\
	                zCharStringtoZSCII(((ulong)(lookupAbbreviationAddr(zMem, xlatChar, zCharString[i+1]))), zMem);
	                for(int i=0; abbrevStr[i]!=0; i++)
	                    zsciiString.push_back(abbrevStr[i]);
	                delete[] abbrevStr;
	                i++;
	                alphaShift=temp.alphaShift;
	                shiftFlag=temp.shiftFlag;
	              }
                continue;
            }else if(xlatChar==3){
                zCharStringtoZSCIIHelper_saveState temp(alphaShift, shiftFlag); // this is necessary to preserve the state of this function's
                                                                                // static members
                if((i+1)<zStringLength)	// this is to make sure we don't overstep array bounds when there is an incomplete mulit-zchar construct
                {
              	  zchar* abbrevStr=\
	                zCharStringtoZSCII(((ulong)(lookupAbbreviationAddr(zMem, xlatChar, zCharString[i+1]))), zMem);
	                for(int i=0; abbrevStr[i]!=0; i++)
	                    zsciiString.push_back(abbrevStr[i]);
	                delete[] abbrevStr;
	                i++;
	                alphaShift=temp.alphaShift;
	                shiftFlag=temp.shiftFlag;
	              }
                continue;
            }
        }
        {
            if(xlatChar>=6 && xlatChar <= 0x1f)
            {
                // otherwise, it is a valid alphanumeric/symbolic zscii char
                if(!shiftFlag)
                {
                    if(defaultAlpha==ALPHABET_0)
                    {
                        // no alphabet shift, use A0
                        zsciiString.push_back((zword)zAlphaTable0[xlatChar-6]);
                    }else if (defaultAlpha==ALPHABET_1){
                        zsciiString.push_back((zword)zAlphaTable1[xlatChar-6]);
                    }else if (defaultAlpha==ALPHABET_2){
                        zsciiString.push_back((zword)zAlphaTable2[xlatChar-6]);
                    }
                }else{
                    // if shift flag is on
                    if(alphaShift==ALPHABET_0)
                    {
                        // no alphabet shift, use A0
                        zsciiString.push_back((zword)zAlphaTable0[xlatChar-6]);
                    }else if (alphaShift==ALPHABET_1){
                        zsciiString.push_back((zword)zAlphaTable1[xlatChar-6]);
                    }else if (alphaShift==ALPHABET_2){
                        zsciiString.push_back((zword)zAlphaTable2[xlatChar-6]);
                    }
                    shiftFlag=false;
                    alphaShift=ALPHABET_0;
                }
            }else{
                //zsciiString.push_back(' ');
                shiftFlag=false;
                alphaShift=ALPHABET_0;
            }
        }
    }
    zsciiString.push_back(NULL);
    zchar* outData=new zchar[zsciiString.size()];
    int j=zsciiString.size();
    for(int i=0; i<j; i++) outData[i]=zsciiString[i];
    return outData;
}

int ZSCIIGetResidentAlphabet(zchar zch) throw (IllegalZCharException)
{
    // returns the resident alphabet of
    // zch (either ALPHABET_0, ALPHABET_1, ALPHABET_2)
    // or throws IllegalZCharException()
    // or returns -1 for NULL char (space)
    // check for alphabet 0:
    if(zch==' ')
        return defaultAlpha;
    for(int i=0; zAlphaTable0[i]!=NULL;i++)
    {
        if(zAlphaTable0[i]==zch)
        {
            return ALPHABET_0;
        }
    }
    for(int i=0; zAlphaTable1[i]!=NULL; i++)
    {
        if(zAlphaTable1[i]==zch)
        {
            return ALPHABET_1;
        }
    }
    for(int i=0; zAlphaTable2[i]!=NULL; i++)
    {
        if(zAlphaTable2[i]==zch)
        {
            return ALPHABET_2;
        }
    }
    THROW_ILLEGALZCHAREXCEPTION(__LINE__, __FUNCTION__, __FILE__);
    return 0;
}

zchar getZCharAlphaShiftCharacter(int currentAlpha, int desiredAlpha, bool shiftLock)
{
    // returns the correct alphabet shift z-character
    // for shifting from currentAlpha to desiredAlpha
    // default parameter shiftLock is ignored for all versions
    // 3 and above
    zchar outChar=NULL;
    int diff=currentAlpha-desiredAlpha;
    if(diff==-1 || diff==2)
    {
        outChar=2;
    }else if(diff==1 || diff==-2){
        outChar=3;
    }
    if(zVersion<3 && shiftLock==true)
    {
        if(outChar==2) outChar=4;
        else if (outChar==3) outChar=5;
    }
    if(zVersion>=3)
    {
        if(outChar==2) outChar=4;
        else if(outChar==3) outChar=5;
    }
    return outChar;
}

int getCharIndex(zchar* str, zchar find)
{
    // helper function for getAlphabetIndex()
    // returns index of a char in str
    for(int i=0; str[i]!=NULL; i++)
    {
        if(str[i]==find)
            return i+1;
    }
    return -1;
}

int getAlphabetIndex(zchar zscii, int alphabet)
{
    // helper function for ZSCIItoZChar()
    // returns the index of zscii in
    // alphabet.
    // if char is not in alphabet, function
    // returns -1.
    if(zscii==' ')
        return 0;
    if(alphabet==ALPHABET_0)
    {
        return getCharIndex((zchar*)zAlphaTable0, zscii);
    }else if(alphabet==ALPHABET_1){
        return getCharIndex((zchar*)zAlphaTable1, zscii);
    }else if(alphabet==ALPHABET_2){
        return getCharIndex((zchar*)zAlphaTable2, zscii);
    }else{
        return -1;
    }
}

zword ZSCIItoZChar(zchar* zscii, int& bytesConverted, bool resetShifts)
{
    // converts 3 or less chars of a ZSCII string to
    // a zword (zchar) string of 3 or less z-chars
    // bytesConverted is a reference to an integer
    // to recieve number of characters actually
    // converted off the zscii string
    static int currentAlpha;
    bool finalCharShiftWasTerminated=true;
    bool isTerminatingWord=false;
    zword outWord=NULL;
    zchar zBytes[3]={0, 0, 0};
    if(resetShifts)
    {
        currentAlpha=defaultAlpha;
        return NULL;
    }
    for(int i=0, j=0; ; i++)
    {
        if(zscii[i]==NULL)
        {
            // reached the end of the zscii string
            for(; j<3; j++)
            {
                zBytes[j]=5;
            }
            bytesConverted=i+1;
            isTerminatingWord=true;
            break; // filled 3 chars
        }
        int resAlpha;
        try{
            if((resAlpha=ZSCIIGetResidentAlphabet(zscii[i]))!=currentAlpha)
            {
                // if this char is not in the same alphabet
                zBytes[j++]=getZCharAlphaShiftCharacter(currentAlpha, resAlpha);
                currentAlpha=resAlpha;
                i--;
                if(j>2)
                {
                    // filled 3 z-chars
                    finalCharShiftWasTerminated=false;
                    bytesConverted=i+1;
                    break;
                }
            }else{
                int alphaIndex;
                if((alphaIndex=getAlphabetIndex(zscii[i], currentAlpha))!=0)
                    alphaIndex+=5;
                zBytes[j++]=alphaIndex;
                if(j>2)
                {
                    if(currentAlpha!=defaultAlpha)
                    {
                        finalCharShiftWasTerminated=true;
                    }
                    bytesConverted=i+1;
                    break;
                }
                currentAlpha=defaultAlpha;
            }
        }catch(IllegalZCharException e){
                zErrorLogger.addError("Error : IllegalZCharException thrown");
                zBytes[j++]=' ';
                if(j>2)
                {
                    bytesConverted=i+1;
                    break;
                }
                currentAlpha=defaultAlpha;
        }
    }
    if(finalCharShiftWasTerminated)
    {
        currentAlpha=defaultAlpha;
    }
    // now we just glue all zBytes[] into a zword
    zchar zUpper, zLower;
    zUpper=zLower=NULL;
    zUpper|=zBytes[0];
    zUpper=(zUpper<<2);
    zUpper|=((zBytes[1]&31)>>3);
    zLower|=(zBytes[1]&7);
    zLower=(zLower<<5);
    zLower|=(zBytes[2]&31);
    outWord=0;
    outWord=((outWord|zUpper)<<8);
    outWord|=zLower;
    if(isTerminatingWord)
    {
        outWord|=32768;
    }
    return (outWord);
}
zword* ZSCIItoZCharString(zchar* zscii)
{
    // converts given zscii string
    // to a string of z-character words
    int zStrlen=ZSCIIStrLen(zscii);
    int charsProcessed=0;
    zword* zCharBuf=new zword[zStrlen*3];
    ZSCIItoZChar(NULL, charsProcessed, true);
    for(int i=0, j=0; i<zStrlen+1; j++)
    {
        zCharBuf[j]=ZSCIItoZChar(&zscii[i], charsProcessed);
        i+=charsProcessed;
    }
    return zCharBuf;
}

zword packZChars(zchar* zchars)
{
	// packs 3x z-characters into a zword
	zword outWord=0;
	outWord|=zchars[0];
	outWord<<=5;
	outWord|=zchars[1];
	outWord<<=5;
	outWord|=zchars[2];
	return (outWord);
}

zword* zChartoDictionaryZCharString(zword* zstring)
{
    // converts a normal zchar string to a dictionary zchar string
    // returns a zword* to either a 2 or 3 zword array.
    std::vector<zchar> vector1=expandZChars(zstring);
    std::vector<zchar> vector2(0);
    if(zVersion<=3){
        // for versions 1 to 3, dictionary word length is 4 bytes (6 zchars)
        int done=0;
        for(int i=0; i<vector1.size() && i<7; i++, done++)
        {
            vector2.push_back(vector1[i]);
        }
        if(done<7)
        {
            for(int i=done; i<7; i++)
            {
                vector2.push_back(5);
            }
        }
        // now we need to pack them to 2x zwords.
        zword* outData=new zword[2];
        zchar* zcharArray=new zchar[vector2.size()];
        for(int i=0; i<vector2.size(); i++) zcharArray[i]=vector2[i];
        outData[0]=(packZChars(zcharArray));
        outData[1]=(packZChars(zcharArray+3)|32768);
        delete[] zcharArray;
        return outData;
    }else if(zVersion>3){
        // for versions 4 and up, dictionary word length is 6 bytes (9 zchars)
        int done=0;
        for(int i=0; i<vector1.size() && i<9; i++, done++)
        {
            vector2.push_back(vector1[i]);
        }
        if(done<9)
        {
            for(int i=done; i<9; i++)
            {
                vector2.push_back(5);
            }
        }
        // now we need to pack them to 3x zwords.
        zword* outData=new zword[3];
        zchar* zcharArray=new zchar[vector2.size()];
        for(int i=0; i<vector2.size(); i++) zcharArray[i]=vector2[i];
        outData[0]=packZChars(zcharArray);
        outData[1]=packZChars(zcharArray+3);
        outData[2]=packZChars(zcharArray+6);
        outData[2]|=32768;
        for(int i=0; i<3; i++) outData[i]=(outData[i]);
        delete[] zcharArray;
        return outData;
    }
    return NULL;
}

zword* dictionaryZCharStringtoZCharString(zword* zstring)
{
    std::vector<zchar> vector1=expandZChars(zstring);
    if(zVersion<=3){
        zword* outData=new zword[2];
        outData[0]=(packZChars(vector1.data()));
        outData[1]=(packZChars(vector1.data()+3)|32768);
        return outData;
    }else if(zVersion>3){
        zword* outData=new zword[9];
        for(int i=0; i<9; i++) outData[i]=(vector1[2*i]<<8)|(vector1[2*i+1]);
        outData[8]|=32768;
        return outData;
    }
    return NULL;
}

zword* dictionaryZCharStringtoZCharString(ulong addr, ZMemory& zMem)
{
    vector<zword> zCharVect(0);
    for(int i=0; ; i+=2)
    {
        zCharVect.push_back(zMem.readZWord(addr+i));
        if(zCharVect[i>>1]>>15) break;
    }
    zword* zCharArr=new zword[zCharVect.size()];
    for(int i=0; i<zCharVect.size(); i++)
    {
        zCharArr[i]=zCharVect[i];
    }
    zword* outData=dictionaryZCharStringtoZCharString(zCharArr);
    delete[] zCharArr;
    return outData;
}
