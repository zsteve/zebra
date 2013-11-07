#include "zstack.h"
#include "..\..\zerror\zerror\zerror.h"

ZStack::ZStack()
{
	stackPtr=NULL;
    stackData=NULL;
    stackSize=0;
}

ZStack::~ZStack()
{
    if(stackData && stackSize)
    {
        delete[] stackData;
    }
}

bool ZStack::isStackEmpty()
{
    if(!stackSize)
        return false;
    else
        return true;
}

bool ZStack::initStack(int size)
{
	if(stackData!=NULL) delete[] stackData;
    stackData=new zword[size];
    stackSize=size;
    stackPtr=size;
    return true;
}

void ZStack::push(zword val) throw (StackFullException)
{
    if(stackPtr==0)
        throw StackFullException(); // if it's full, we throw an exception
    // otherwise, we continue and add it to the stack
    stackPtr--;
    stackData[stackPtr]=val;
    return;
}

zword ZStack::pull() throw (StackEmptyException)
{
    if(stackPtr==stackSize)
    {
        throw StackEmptyException();  // if it's empty, we throw an exception
    }
    // or else we continue
    return stackData[stackPtr++];
}
