#include "ztext.h"


#define ALPHABET_1 1
#define ALPHABET_2 2
#define ALPHABET_0 0

char zAlphaTable0[]={"abcdefghijklmnopqrstuvwxyz"};
char zAlphaTable1[]={"ABCDEFGHIJKLMNOPQRSTUVWXYZ"};
char zAlphaTable2[]={" ^0123456789.,!?_#'\"/-:()"};

int zVersion=3;

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
        if(str[i]>>15)
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
    for(int j=0; cat[j]!=0; j++, i++)
    {
        src[i]=cat[j];
    }

}

zchar* zCharStringtoZSCII(zword* zCharString)
{
    // converts an entire z-char string
    // to a ZSCII string
    zchar* ZSCIIString=new zchar[zCharStrLen(zCharString)+1];
    for(int len=zCharStrLen(zCharString)+1, i=0; i<len; i++)
        ZSCIIString[i]=NULL;
    bool exit=false;
    for(int i=0; ; i)
    {
        // while bit 7 of H.O. byte of zCharString[i]
        // is clear, keep going
        if(zCharString[i]>>15)
            exit=true;
        zchar* out=zChartoZSCII(zCharString[i]);
        ZSCIIStrCat(ZSCIIString, out);
        delete[] out;
        if(exit)
            break;
        i++;
    }
    zChartoZSCII(NULL, true);
    return ZSCIIString;
}
zchar* zChartoZSCII(zword zChar, bool resetShifts)
{
    // converts a single z-character to
    // a string of 3 or less ZSCII characters (in 16-bit zwords)
    // terminated by NULL
    // it is the caller's responsibility to free the
    // resulting value
    static int alphaShift;
    static bool shiftFlag;
    if(resetShifts)
    {
        alphaShift=0;
        shiftFlag=0;
        return NULL;
    }
    // check if bit 7 is on
    zChar&=65535;
    zchar zUpper, zLower;
    zUpper=(zchar)(zChar>>8);
    zLower=(zchar)(zChar&255);
    zchar *zcharString=new zchar[4];
    zchar *zsciiString=new zchar[4];
    for(int i=0; i<4;)
        zsciiString[i++]=NULL;
    zcharString[0]=(zUpper>>2)&31;
    zcharString[1]=((zUpper&3)<<3)|((zLower>>5));
    zcharString[2]=(zLower&31);
    zcharString[3]=NULL;
    // now we map the z-characters to the ZSCII equivalents
    for(int j=0, i=0; i<3; i++)
    {
        char xlatChar=zcharString[i];
        if(xlatChar==NULL)
        {
            // NULL == space
            zsciiString[j++]=' ';
            shiftFlag=false;
            alphaShift=ALPHABET_0;
            continue;
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
                /**** TODO ****/
                // Add support for abbreviations
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

            }else if(xlatChar==2){

            }else if(xlatChar==3){

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
                        zsciiString[j++]=(zword)zAlphaTable0[xlatChar-6];
                    }else if (defaultAlpha==ALPHABET_1){
                        zsciiString[j++]=(zword)zAlphaTable1[xlatChar-6];
                    }else if (defaultAlpha==ALPHABET_2){
                        zsciiString[j++]=(zword)zAlphaTable2[xlatChar-6];
                    }
                }else{
                    // if shift flag is on
                    if(alphaShift==ALPHABET_0)
                    {
                        // no alphabet shift, use A0
                        zsciiString[j++]=(zword)zAlphaTable0[xlatChar-6];
                    }else if (alphaShift==ALPHABET_1){
                        zsciiString[j++]=(zword)zAlphaTable1[xlatChar-6];
                    }else if (alphaShift==ALPHABET_2){
                        zsciiString[j++]=(zword)zAlphaTable2[xlatChar-6];
                    }
                    shiftFlag=false;
                    alphaShift=ALPHABET_0;
                }
            }else{
                zsciiString[j++]=' ';
                shiftFlag=false;
                alphaShift=ALPHABET_0;
            }
        }
    }
    return zsciiString;
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
    throw IllegalZCharException();
}

zchar getZCharAlphaShiftCharacter(int currentAlpha, int desiredAlpha, bool shiftLock)
{
    // returns the correct alphabet shift z-character
    // for shifting from currentAlpha to desiredAlpha
    // default parameter shiftLock is ignored for all versions
    // 3 and above
    zchar outChar;
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

zword ZSCIItoZChar(zchar* zscii, int& bytesConverted)
{
    // converts 3 or less chars of a ZSCII string to
    // a zword (zchar) string of 3 or less z-chars
    // bytesConverted is a reference to an integer
    // to recieve number of characters actually
    // converted off the zscii string
    int currentAlpha=defaultAlpha;
    bool isTerminatingWord=false;
    zword outWord=NULL;
    zchar zBytes[3]={0, 0, 0};
    for(int i=0, j=0; ; i++)
    {
        if(zscii[i]==NULL)
        {
            // reached the end of the zscii string
            for(; j<2; j++)
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
    return outWord;
}
zword* ZSCIItoZCharString(zchar* zscii)
{
    // converts given zscii string
    // to a string of z-character words
    int zStrlen=ZSCIIStrLen(zscii);
    int charsProcessed=0;
    zword* zCharBuf=new zword[zStrlen*3];
    for(int i=0, j=0; i<zStrlen+1; j++)
    {
        zCharBuf[j]=ZSCIItoZChar(&zscii[i], charsProcessed);
        i+=charsProcessed;
    }
    return zCharBuf;
}
