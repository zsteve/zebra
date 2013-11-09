#ifndef ZGLOBAL_H
#define ZGLOBAL_H
#include "../zerror/zerror/zerror.h"
#include <vector>
#include <exception>
#include <string>
#include <cstdio>
#include <cstring>
#include <cstdlib>

using namespace std;

// typedefs
typedef unsigned short zword;
typedef unsigned char zchar;
typedef unsigned char zbyte;
typedef unsigned long ulong;

// functions

zword endianize(zword in);
zword reverseBitSequence(zbyte in);

// classes

class ZException : exception{
    protected:
    string compileErrorMsg(const int line, const char* function, const char* file){
        string errorMsg("");
        char* lineNumber=new char[20];
        sprintf(lineNumber, "line : %d ", line);
        errorMsg+=lineNumber;
        delete[] lineNumber;
        errorMsg+="In function : ";
        errorMsg+=function;
        errorMsg+=" In file : ";
        errorMsg+=file;
        return errorMsg;
    }
};

#endif
