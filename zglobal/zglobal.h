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
zword* endianizeString(zword* in);
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

// template functions
template <typename T>
vector<T> arrayToVector(T* array, int size)
{
    if(size==0) return;
    vector<T> outVect(size);
    for(int i=0; i<size; i++)
    {
        outVect[i]=array[i];
    }
    return outVect;
}

template<zword>
vector<zword> arrayToVector(zword* array)
{
    vector<zword> outVect(0);
    for(int i=0; ; i++)
    {
        outVect.push_back(array[i]);
        if(outVect[i]>>15) break;
    }
    return outVect;
}

template<zchar>
vector<zchar> arrayToVector(zchar* array)
{
    vector<zchar> outVect(0);
    int i=0;
    for(int i=0; ; i++)
    {
        outVect.push_back(array[i]);
        if(array[i]==NULL) break;
    }
    return outVect;
}

template <typename T>
T* vectorToArray(vector<T> vect)
{
    if(vect.size()==0) return NULL;
    T* outArray=new T[vect.size()];
    for(int i=0; i<vect.size(); i++)
    {
        outArray[i]=vect[i];
    }
    return outArray;
}

#endif
