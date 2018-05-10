#ifndef ZSTACK_H
#define ZSTACK_H
#include <exception>
#include "../zglobal/zglobal.h"
#include "../zerror/zerror.h"

// exception classes

#define THROW_STACKEMPTYEXCEPTION(line, function, file)\
    throw StackEmptyException((const int)line, (const char*)function, (const char*)file);

#define THROW_STACKFULLEXCEPTION(line, function, file)\
    throw StackFullException((const int)line, (const char*)function, (const char*)file);

extern ZError zErrorLogger;

class StackEmptyException : ZException{
    public:
    StackEmptyException()
    {
        zErrorLogger.addError("Error : StackEmptyException thrown");
    }
    StackEmptyException(const int line, const char* function, const char* file){
        zErrorLogger.addError(("Error : StackEmptyException thrown : "+compileErrorMsg(line, function, file)).c_str());
    }
};

class StackFullException : ZException{
    public:
    StackFullException()
    {
        zErrorLogger.addError("Error : StackFullException thrown");
    }
    StackFullException(const int line, const char* function, const char* file){
        zErrorLogger.addError(("Error : StackFullException thrown : "+compileErrorMsg(line, function, file)).c_str());
    }
};

// stack class

class ZStack{
    private:
    zword *stackData;
    ulong stackPtr;
    int stackSize;

    public:
    ZStack();
    ~ZStack();
    bool isStackEmpty();
    bool initStack(int size);
    void push(zword val) throw (StackFullException);
    zword pull() throw (StackEmptyException);

    void clearStack();
	// accessor functions
	ulong getStackPtr(){return stackPtr;}
	void setStackPtr(ulong newPtr){stackPtr=newPtr;}
	zword* getStackData(){return stackData;}
	ulong getStackSize(){return stackSize;}

    protected:
	};
#endif
