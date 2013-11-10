#include "zinout.h"

void ZInOut::print(char* str){
    #ifdef PLATFORM_LINUX || PLATFORM_WIN32
    printf(str);
    #endif
}
