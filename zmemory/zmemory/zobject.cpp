#include "zobject.h"

extern int zVersion;

ZObjectTable::ZObjectTable()
{
    zMemObjPtr=NULL;
    zObjectTable=NULL;
    zObjectTree=NULL;
}

ZObjectTable::ZObjectTable(ZMemory* zMemObj)
{
    zMemObjPtr=zMemObj;
    zObjectTable=zMemObjPtr->readZWord(0xA);
    // find base of the object tree
    if(zVersion<=3)
    {
        zObjectTree=zObjectTable+31*2;
    }else if(zVersion>3){
        zObjectTree=zObjectTable+63*2;
    }
}

ZObjectTable::~ZObjectTable()
{

}

zword ZObjectTable::getDefaultProperty(ulong index) throw (IllegalPropertyIndex)
{
    // returns the zword in the default property index (offset stored at 0xa in header)
    // for index.
    // throws IllegalPropertyIndex if the index is invalid.
    if(zVersion<=3)
    {
        if(index>31)
        {
            throw IllegalPropertyIndex();
        }
        return zMemObjPtr->readZWord(zObjectTable+(index<<1));
    }else if(zVersion>3){
        if(index>63)
        {
            throw IllegalPropertyIndex();
        }
        return zMemObjPtr->readZWord(zObjectTable+(index<<1));
    }
}

ulong ZObjectTable::getObjectAddr(ulong index) throw (IllegalObjectIndex)
{
    // returns the byte address of a certain object
    // in the object tree
    // (helper function)
    if(zVersion<=3){
        // max 255 objects for versions 1, 2, 3
        if(index>255 || index==0)
        {
            throw IllegalObjectIndex();
        }
        ulong addr=zObjectTree+((index-1)*9);  // 9 byte entry for object
        return addr;
    }else if(zVersion>3){
        if(index>65536 || index==0)
        {
            throw IllegalObjectIndex();
        }
        ulong addr=zObjectTree+((index-1)*14); // 14 byte entry for object
        return addr;
    }
}

ulong ZObjectTable::getObjectAttributeFlags32(ulong index) throw (IllegalObjectIndex)
{
    // returns the 32-bit attribute flags for object[index]
    // throws IllegalObjectIndex for illegal index
    if(((index>255 || index==0) && zVersion<=3))
    {
        throw IllegalObjectIndex();
    }
    if(zVersion>3)
    {
        throw IllegalObjectIndex();
    }
    zbyte attrib[4];
    try{
        attrib[0]=zMemObjPtr->readZByte(getObjectAddr(index)+0);
        attrib[1]=zMemObjPtr->readZByte(getObjectAddr(index)+1);
        attrib[2]=zMemObjPtr->readZByte(getObjectAddr(index)+2);
        attrib[3]=zMemObjPtr->readZByte(getObjectAddr(index)+3);
    }catch(ZMemoryReadOutOfBounds e){
        throw IllegalObjectIndex();
    }
    ulong outLong=0;
    outLong|=reverseBitSequence(attrib[3]);
    outLong=outLong<<8;
    outLong|=reverseBitSequence(attrib[2]);
    outLong=outLong<<8;
    outLong|=reverseBitSequence(attrib[1]);
    outLong=outLong<<8;
    outLong|=reverseBitSequence(attrib[0]);
    return outLong;
}

ulong ZObjectTable::getObjectAttributeFlags48(ulong index) throw (IllegalObjectIndex)
{
    // TODO : ADD SUPPORT FOR 48 BIT ATTRIBUTE FLAGS (VERSIONS 4 AND UP)
}

zword ZObjectTable::getObjectParent(ulong index) throw (IllegalObjectIndex)
{
    // returns the byte address of a given object's parent
    if(((index>255 || index==0) && zVersion<=3) || ((index>65535 || index==0) && zVersion>3))
    {
        throw IllegalObjectIndex();
    }
    zbyte objectParent=0;
    try{
        if(zVersion<=3)
        {
            objectParent=(zword)zMemObjPtr->readZByte(getObjectAddr(index)+4);
        }else if(zVersion>3)
        {
            objectParent=zMemObjPtr->readZWord(getObjectAddr(index)+7);
        }
    }catch(ZMemoryReadOutOfBounds e){
        throw IllegalObjectIndex();
    }catch(IllegalObjectIndex e){
        throw IllegalObjectIndex();
    }
    return objectParent;
}

zword ZObjectTable::getObjectSibling(ulong index) throw (IllegalObjectIndex)
{
    // returns the byte address of a given object's sibling
    if(((index>255 || index==0) && zVersion<=3) || ((index>65535 || index==0) && zVersion>3))
    {
        throw IllegalObjectIndex();
    }
    zbyte objectSibling=0;
    try{
        if(zVersion<=3)
        {
            objectSibling=(zword)zMemObjPtr->readZByte(getObjectAddr(index)+5);
        }else if(zVersion>3)
        {
            objectSibling=zMemObjPtr->readZWord(getObjectAddr(index)+9);
        }
    }catch(ZMemoryReadOutOfBounds e){
        throw IllegalObjectIndex();
    }catch(IllegalObjectIndex e){
        throw IllegalObjectIndex();
    }
    return objectSibling;
}

zword ZObjectTable::getObjectChild(ulong index) throw (IllegalObjectIndex)
{
    // returns the byte index of a given object's child
    if(((index>255 || index==0) && zVersion<=3) || ((index>65535 || index==0) && zVersion>3))
    {
        throw IllegalObjectIndex();
    }
    zword objectChild=0;
    try{
        if(zVersion<=3)
        {
            objectChild=(zword)zMemObjPtr->readZByte(getObjectAddr(index)+6);
        }else if(zVersion>3)
        {
            objectChild=zMemObjPtr->readZWord(getObjectAddr(index)+11);
        }
    }catch(ZMemoryReadOutOfBounds e){
        throw IllegalObjectIndex();
    }catch(IllegalObjectIndex e){
        throw IllegalObjectIndex();
    }
    return objectChild;
}

zword ZObjectTable::setObjectParent(ulong indexChild, ulong indexParent) throw (IllegalObjectIndex)
{
    // sets object[indexParent]'s parent to indexChild, and returns the previous parent.
    if(((indexChild>255 || indexChild==0) && zVersion<=3) || ((indexChild>65535 || indexChild==0) && zVersion>3))
    {
        throw IllegalObjectIndex();
    }
    if((indexParent>255&& zVersion<=3) || (indexParent>65535 && zVersion>3))
    {
        throw IllegalObjectIndex();
    }
    zword oldParent;
    try{
        if(zVersion<=3)
        {
            oldParent=getObjectParent(indexChild);
            zMemObjPtr->storeZByte(getObjectAddr(indexChild)+4, indexParent);
        }else if(zVersion>3){
            oldParent=getObjectChild(indexChild);
            zMemObjPtr->storeZWord(getObjectAddr(indexChild)+7, indexParent);
        }
    }catch(ZMemoryWriteOutOfBounds e){
        throw IllegalObjectIndex();
    }catch(IllegalObjectIndex e){
        throw e;
    }
    return oldParent;
}

zword ZObjectTable::setObjectSibling(ulong indexObject, ulong indexSibling) throw (IllegalObjectIndex)
{
    // sets object[indexSibling]'s sibling to indexObject, and returns the previous sibling.
    if(((indexObject>255 || indexObject==0) && zVersion<=3) || ((indexObject>65535 || indexObject==0) && zVersion>3))
    {
        throw IllegalObjectIndex();
    }
    if((indexSibling>255&& zVersion<=3) || (indexSibling>65535 && zVersion>3))
    {
        throw IllegalObjectIndex();
    }
    zword oldSibling;
    try{
        if(zVersion<=3)
        {
            oldSibling=getObjectParent(indexObject);
            zMemObjPtr->storeZByte(getObjectAddr(indexObject)+5, indexSibling);
        }else if(zVersion>3){
            oldSibling=getObjectChild(indexObject);
            zMemObjPtr->storeZWord(getObjectAddr(indexObject)+9, indexSibling);
        }
    }catch(ZMemoryWriteOutOfBounds e){
        throw IllegalObjectIndex();
    }catch(IllegalObjectIndex e){
        throw e;
    }
    return oldSibling;
}

zword ZObjectTable::setObjectChild(ulong indexParent, ulong indexChild) throw (IllegalObjectIndex)
{
    // sets object[indexParent]'s child to indexChild, and returns the previous child.
    if(((indexParent>255 || indexParent==0) && zVersion<=3) || ((indexParent>65535 || indexParent==0) && zVersion>3))
    {
        throw IllegalObjectIndex();
    }
    if((indexChild>255&& zVersion<=3) || (indexChild>65535 && zVersion>3))
    {
        throw IllegalObjectIndex();
    }
    zword oldChild;
    try{
        if(zVersion<=3)
        {
            oldChild=getObjectChild(indexParent);
            zMemObjPtr->storeZByte(getObjectAddr(indexParent)+6, indexChild);
        }else if(zVersion>3){
            oldChild=getObjectChild(indexParent);
            zMemObjPtr->storeZWord(getObjectAddr(indexParent)+11, indexChild);
        }
    }catch(ZMemoryWriteOutOfBounds e){
        throw IllegalObjectIndex();
    }catch(IllegalObjectIndex e){
        throw e;
    }
    return oldChild;
}

zword ZObjectTable::getObjectPropertyListAddr(ulong index) throw (IllegalObjectIndex)
{
    // returns the word address of object's property list
    if(((index>255 || index==0) && zVersion<=3) || ((index>65535 || index==0) && zVersion>3))
    {
        throw IllegalObjectIndex();
    }
    zword listAddr=0;
    try{
        if(zVersion<=3)
        {
            listAddr=zMemObjPtr->readZWord(getObjectAddr(index)+7);
        }else if(zVersion>3){
            listAddr=zMemObjPtr->readZWord(getObjectAddr(index)+13);
        }
    }catch(ZMemoryReadOutOfBounds e){
        throw IllegalObjectIndex();
    }catch(IllegalObjectIndex e){
        throw IllegalObjectIndex();
    }
    return listAddr;
}
