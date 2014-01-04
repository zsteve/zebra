#include "zinout.h"
#include <iostream>
#include <cstdlib>
#include <cstdio>

#ifdef PLATFORM_WIN32_CONSOLE
#include <conio.h>
#endif

void ZInOut::print(char* str){
   // #ifdef PLATFORM_LINUX_CONSOLE || PLATFORM_WIN32_CONSOLE
    printf(str);
   // #endif
}

void ZInOut::readLine(char* buffer){
  // #ifdef PLATFORM_LINUX_CONSOLE || PLATFORM_WIN32_CONSOLE
    scanf("%s", buffer);
   // #endif
}

char* ZInOut::readLine(){
   // #ifdef PLATFORM_LINUX_CONSOLE || PLATFORM_WIN32_CONSOLE
    static char* buffer=NULL;
    if(buffer){
        delete[] buffer;
    }
    buffer=new char[80];
    gets(buffer);
    return buffer;
    //#endif
}

char ZInOut::getChar(){
	#ifdef PLATFORM_WIN32_CONSOLE
	return _getch();
	#elif defined PLATFORM_LINUX_CONSOLE
	return getchar();
	#endif
}
