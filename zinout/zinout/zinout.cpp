#include "zinout.h"

void ZInOut::print(char* str){
    #ifdef PLATFORM_LINUX || PLATFORM_WIN32 && SYSTEM CONSOLE
    printf(str);
    #endif
}

void ZInOut::readLine(char* buffer){
    #ifdef PLATFORM_LINUX || PLATFORM_WIN32 && SYSTEM CONSOLE
    scanf("%s", buffer);

    #endif
}

char* ZInOut::readLine(){
    #ifdef PLATFORM_LINUX || PLATFORM_WIN32 && SYSTEM_CONSOLE
    static char* buffer=NULL;
    if(buffer){
        delete[] buffer;
    }
    buffer=new char[80];
    scanf("%s", buffer);
    return buffer;
    #endif
}
