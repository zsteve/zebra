#include "zerror.h"
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
