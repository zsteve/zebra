#include <iostream>
#include <cstdio>
#include "../../zmemory/zmemory/zmemory.h"
#include "../../zmemory/zmemory/zobject.h"
#include "../../zerror/zerror/zerror.h"
#include "ztext.h"
#include <conio.h>
#include <vector>
#include <exception>

using namespace std;

zchar zstring[11] = {
	0x1F, 0x51, 0x31, 0xD3, 0x30, 0x08, 0x35, 0x58, 0xE4, 0xA5, 0x79
};

int zVersion;

long filesize(FILE *stream)
{
   long curpos, length;
   curpos = ftell(stream);
   fseek(stream, 0L, SEEK_END);
   length = ftell(stream);
   fseek(stream, curpos, SEEK_SET);
   return length;
}

int main()
{
    cout << "Z-Text unit test!" << endl;

    FILE* storyFile=fopen("zork2.z3", "r");
    if(storyFile==NULL) return -1;
    zbyte* storyData=new zbyte[filesize(storyFile)];
    fread(storyData, filesize(storyFile), 1, storyFile);
    ZMemory zMem(storyData, filesize(storyFile));
    ZObjectTable zObj(&zMem);
    cout << zCharStringtoZSCII(ZSCIItoZCharString((zchar*)"A new line : \nand a new line : \n"), zMem) << endl;
    zword* zstr_w=(zword*)zstring;
    cout << zCharStringtoZSCII(zstr_w, zMem) << endl;
    cout << ZSCIIGetResidentAlphabet('-') << endl;
    cout << (int)getZCharAlphaShiftCharacter(0, 2) << endl;
    int a=0;
    zword out;
    cout << (out=ZSCIItoZChar((zchar*)"Grue gets you ", a)) << endl;

    cout << zCharStringtoZSCII((zword*)(zMem.getRawDataPtr()+0x15140), zMem);
    getch();
    return 0;
}
