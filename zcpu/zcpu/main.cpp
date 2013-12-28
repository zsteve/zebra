#include <iostream>
#include <cstdlib>
#include "zcpu.h"
#include "../../zmemory/zmemory/zmemory.h"

int zVersion=3;

using namespace std;

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
    FILE* storyFile=fopen("zork2.z3", "r");
    if(storyFile==NULL) return -1;
    zbyte* storyData=new zbyte[filesize(storyFile)];
    fread(storyData, filesize(storyFile), 1, storyFile);
    ZMemory zMem(storyData, filesize(storyFile));
	ZObjectTable zObj(&zMem);
	ZStack zStack;
	ZInOut zInOut;
	ZCpu c(zMem, zStack, zObj, zInOut);
	c.startExecution();
    return 0;
}