#include "zerror.h"
#include "../zinout/zinout.h"
#include <cstdio>

void ZError::addError(string errorMsg)
{
    buffer+=errorMsg+"\n";
    if(flushType==FLUSH_IMMEDIATE)
    {
        FILE* file;
        file=fopen("debug.log", "a");
        fprintf(file, "%s", buffer.c_str());
        fclose(file);
		buffer.clear();
    }
    return;
}

void ZError::addError(string errorMsg, int errorLevel)
{
    buffer+=errorMsg+"\n";
	if(errorLevel==ERROR_FATAL){
		cout << "Fatal Error : " << errorMsg << endl;
	}
    if(flushType==FLUSH_IMMEDIATE)
    {
        FILE* file;
        file=fopen("debug.log", "a");
        fprintf(file, "%s", buffer.c_str());
        fclose(file);
		buffer.clear();
    }
    return;
}

void ZError::setBufferFlush(int val)
{
    flushType=val;
}

void ZError::flushBuffer()
{
    FILE* file;
    file=fopen("debug.log", "a");
    fprintf(file, "%s", buffer.c_str());
    fclose(file);
}
