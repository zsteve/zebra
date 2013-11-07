#ifndef ZSTACK_H
#define ZSTACK_H
#include <exception>
#include "..\..\zglobal\zglobal.h"

// exception classes


class StackEmptyException : std::exception{
    public:
    StackEmptyException()
    {
        //zErrorLog.addError("Error : StackEmptyException thrown");
    }
};

class StackFullException : std::exception{
    public:
    StackFullException()
    {
        //zErrorLog.addError("Error : StackFullException thrown");
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
    protected:
};
#endif
