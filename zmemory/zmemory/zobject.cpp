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
            THROW_ILLEGALPROPERTYINDEX(__LINE__, __FUNCTION__, __FILE__);
        }
        return zMemObjPtr->readZWord(zObjectTable+((index-1)<<1));
    }else if(zVersion>3){
        if(index>63)
        {
            THROW_ILLEGALPROPERTYINDEX(__LINE__, __FUNCTION__, __FILE__);
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
            THROW_ILLEGALOBJECTINDEX(__LINE__, __FUNCTION__, __FILE__);
        }
        ulong addr=zObjectTree+((index-1)*9);  // 9 byte entry for object
        return addr;
    }else if(zVersion>3){
        if(index>65536 || index==0)
        {
            THROW_ILLEGALOBJECTINDEX(__LINE__, __FUNCTION__, __FILE__);
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
        THROW_ILLEGALOBJECTINDEX(__LINE__, __FUNCTION__, __FILE__);
    }
    if(zVersion>3)
    {
        THROW_ILLEGALOBJECTINDEX(__LINE__, __FUNCTION__, __FILE__);
    }
    zbyte attrib[4];
    try{
        attrib[0]=zMemObjPtr->readZByte(getObjectAddr(index)+0);
        attrib[1]=zMemObjPtr->readZByte(getObjectAddr(index)+1);
        attrib[2]=zMemObjPtr->readZByte(getObjectAddr(index)+2);
        attrib[3]=zMemObjPtr->readZByte(getObjectAddr(index)+3);
    }catch(ZMemoryReadOutOfBounds e){
        THROW_ILLEGALOBJECTINDEX(__LINE__, __FUNCTION__, __FILE__);
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
	return NULL;
}

zword ZObjectTable::getObjectParent(ulong index) throw (IllegalObjectIndex)
{
    // returns the byte address of a given object's parent
    if(((index>255 || index==0) && zVersion<=3) || ((index>65535 || index==0) && zVersion>3))
    {
        THROW_ILLEGALOBJECTINDEX(__LINE__, __FUNCTION__, __FILE__);
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
        THROW_ILLEGALOBJECTINDEX(__LINE__, __FUNCTION__, __FILE__);
    }catch(IllegalObjectIndex e){
        THROW_ILLEGALOBJECTINDEX(__LINE__, __FUNCTION__, __FILE__);
    }
    return objectParent;
}

zword ZObjectTable::getObjectSibling(ulong index) throw (IllegalObjectIndex)
{
    // returns the byte address of a given object's sibling
    if(((index>255 || index==0) && zVersion<=3) || ((index>65535 || index==0) && zVersion>3))
    {
        THROW_ILLEGALOBJECTINDEX(__LINE__, __FUNCTION__, __FILE__);
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
        THROW_ILLEGALOBJECTINDEX(__LINE__, __FUNCTION__, __FILE__);
    }catch(IllegalObjectIndex e){
        THROW_ILLEGALOBJECTINDEX(__LINE__, __FUNCTION__, __FILE__);
    }
    return objectSibling;
}

zword ZObjectTable::getObjectChild(ulong index) throw (IllegalObjectIndex)
{
    // returns the byte index of a given object's child
    if(((index>255 || index==0) && zVersion<=3) || ((index>65535 || index==0) && zVersion>3))
    {
        THROW_ILLEGALOBJECTINDEX(__LINE__, __FUNCTION__, __FILE__);
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
        THROW_ILLEGALOBJECTINDEX(__LINE__, __FUNCTION__, __FILE__);
    }catch(IllegalObjectIndex e){
        THROW_ILLEGALOBJECTINDEX(__LINE__, __FUNCTION__, __FILE__);
    }
    return objectChild;
}

zword ZObjectTable::setObjectParentHelper(ulong indexChild, ulong indexParent) throw (IllegalObjectIndex)
{
    // sets object[indexParent]'s parent to indexChild, and returns the previous parent.
    if(((indexChild>255 || indexChild==0) && zVersion<=3) || ((indexChild>65535 || indexChild==0) && zVersion>3))
    {
        THROW_ILLEGALOBJECTINDEX(__LINE__, __FUNCTION__, __FILE__);
    }
    if((indexParent>255&& zVersion<=3) || (indexParent>65535 && zVersion>3))
    {
        THROW_ILLEGALOBJECTINDEX(__LINE__, __FUNCTION__, __FILE__);
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
        THROW_ILLEGALOBJECTINDEX(__LINE__, __FUNCTION__, __FILE__);
    }catch(IllegalObjectIndex e){
        throw e;
    }
    return oldParent;
}

zword ZObjectTable::setObjectSiblingHelper(ulong indexObject, ulong indexSibling) throw (IllegalObjectIndex)
{
    // sets object[indexSibling]'s sibling to indexObject, and returns the previous sibling.
    if(((indexObject>255 || indexObject==0) && zVersion<=3) || ((indexObject>65535 || indexObject==0) && zVersion>3))
    {
        THROW_ILLEGALOBJECTINDEX(__LINE__, __FUNCTION__, __FILE__);
    }
    if((indexSibling>255&& zVersion<=3) || (indexSibling>65535 && zVersion>3))
    {
        THROW_ILLEGALOBJECTINDEX(__LINE__, __FUNCTION__, __FILE__);
    }
    zword oldSibling;
    try{
        if(zVersion<=3)
        {
            oldSibling=getObjectSibling(indexObject);
            zMemObjPtr->storeZByte(getObjectAddr(indexObject)+5, indexSibling);
        }else if(zVersion>3){
            oldSibling=getObjectSibling(indexObject);
            zMemObjPtr->storeZWord(getObjectAddr(indexObject)+9, indexSibling);
        }
    }catch(ZMemoryWriteOutOfBounds e){
        THROW_ILLEGALOBJECTINDEX(__LINE__, __FUNCTION__, __FILE__);
    }catch(IllegalObjectIndex e){
        throw e;
    }
    return oldSibling;
}

zword ZObjectTable::setObjectChildHelper(ulong indexParent, ulong indexChild) throw (IllegalObjectIndex)
{
    // sets object[indexParent]'s child to indexChild, and returns the previous child.
    if(((indexParent>255 || indexParent==0) && zVersion<=3) || ((indexParent>65535 || indexParent==0) && zVersion>3))
    {
        THROW_ILLEGALOBJECTINDEX(__LINE__, __FUNCTION__, __FILE__);
    }
    if((indexChild>255&& zVersion<=3) || (indexChild>65535 && zVersion>3))
    {
        THROW_ILLEGALOBJECTINDEX(__LINE__, __FUNCTION__, __FILE__);
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
        THROW_ILLEGALOBJECTINDEX(__LINE__, __FUNCTION__, __FILE__);
    }catch(IllegalObjectIndex e){
        throw e;
    }
    return oldChild;
}

zword ZObjectTable::setObjectParent(ulong indexChild, ulong indexParent) throw (IllegalObjectIndex)
{
	//if(indexParent){
	//	setObjectChildHelper(indexParent, indexChild);
	//}
    return setObjectParentHelper(indexChild, indexParent);
}

zword ZObjectTable::setObjectSibling(ulong indexObject, ulong indexSibling) throw (IllegalObjectIndex)
{
    return setObjectSiblingHelper(indexObject, indexSibling);
}

zword ZObjectTable::setObjectChild(ulong indexParent, ulong indexChild) throw (IllegalObjectIndex)
{
	//if(indexChild){
	//	setObjectParentHelper(indexChild, indexParent);
	//}
    return setObjectChildHelper(indexParent, indexChild);
}

void ZObjectTable::setObjectAttributeFlags32(ulong index, ulong flags) throw (IllegalObjectIndex)
{
    if(((index>255 || index==0) && zVersion<=3))
    {
        THROW_ILLEGALOBJECTINDEX(__LINE__, __FUNCTION__, __FILE__);
    }
    if(zVersion>3)
    {
        THROW_ILLEGALOBJECTINDEX(__LINE__, __FUNCTION__, __FILE__);
    }
	zbyte attrib[4];
	attrib[0]=(flags&0xFF);
	attrib[1]=(flags&0xFF00)>>8;
	attrib[2]=(flags&0xFF0000)>>16;
	attrib[3]=(flags&0xFF000000)>>24;
	zMemObjPtr->storeZByte(getObjectAddr(index)+0, reverseBitSequence(attrib[0]));
	zMemObjPtr->storeZByte(getObjectAddr(index)+1, reverseBitSequence(attrib[1]));
	zMemObjPtr->storeZByte(getObjectAddr(index)+2, reverseBitSequence(attrib[2]));
	zMemObjPtr->storeZByte(getObjectAddr(index)+3, reverseBitSequence(attrib[3]));
	return;
}

zword ZObjectTable::getObjectPropertyHeaderAddr(ulong index) throw (IllegalObjectIndex)
{
    // returns the word address of object's property list
    if(((index>255 || index==0) && zVersion<=3) || ((index>65535 || index==0) && zVersion>3))
    {
        THROW_ILLEGALOBJECTINDEX(__LINE__, __FUNCTION__, __FILE__);
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
        THROW_ILLEGALOBJECTINDEX(__LINE__, __FUNCTION__, __FILE__);
    }catch(IllegalObjectIndex e){
        THROW_ILLEGALOBJECTINDEX(__LINE__, __FUNCTION__, __FILE__);
    }
    return listAddr;
}

zword ZObjectTable::getObjectPropertyListAddr(ulong index) throw (IllegalObjectIndex)
{
    // returns the word address of object's property list
    if(((index>255 || index==0) && zVersion<=3) || ((index>65535 || index==0) && zVersion>3))
    {
        THROW_ILLEGALOBJECTINDEX(__LINE__, __FUNCTION__, __FILE__);
    }
    try{
        zword addr=getObjectPropertyHeaderAddr(index);
        zbyte textLength=zMemObjPtr->readZByte(addr+0);
        addr+=textLength*2+1;
        return addr;
    }catch(ZMemoryReadOutOfBounds e){
        throw e;
    }catch (IllegalObjectIndex e){
        throw e;
    }
}

zword* ZObjectTable::getObjectName(ulong index) throw (IllegalObjectIndex)
{
    // returns a zword pointer to the z-character string of given object's name
    try{
        ulong addr=getObjectPropertyHeaderAddr(index);
        zbyte nameLength=zMemObjPtr->readZByte(addr+0);
        zword* zString=new zword[nameLength];
        for(int i=0; i<(int)nameLength; i++)
        {
            zString[i]=(zMemObjPtr->readZWord(addr+1+(2*i)));
        }
        return zString;
    }catch(ZMemoryReadOutOfBounds e){
        throw e;
    }catch(IllegalObjectIndex e){
        throw e;
    }
}

zbyte ZObjectTable::getPropertySize(zword addr) throw (ZMemoryReadOutOfBounds)
{
    // returns the byte size of a given property, not including its size byte
    // this version for versions 1-3
    try{
        zbyte size=zMemObjPtr->readZByte(addr);
        if(size==0) return size;
        size=(size/32);
        return size+1;
    }catch(ZMemoryReadOutOfBounds e){
        throw e;
    }
};

zword ZObjectTable::getPropertySize(zword addr, bool &isWordSizeFlag) throw (ZMemoryReadOutOfBounds)
{
    // returns the byte size of a given property, not including its size byte(s)
    // this version for versions 4 and up
    // isWordSizeFlag is a reference to a bool to hold true/false for
    // whether the property uses a size byte (false) or size word (true)
    try{
        zbyte size=zMemObjPtr->readZByte(addr);
        if(size&128){
            // if bit 7 is set, there is a second byte
            isWordSizeFlag=true;
            zbyte size2=zMemObjPtr->readZByte(addr+0);
            // size is in 2nd byte, for bits 0 to 5
            return (size2&63);
        }else{
            // if bit 7 is clear, there is only one byte
            // bit 6 : set for size == 1, clear for size==2
            isWordSizeFlag=false;
            size=(size&64)>>6;
            size=((size==1) ? 2 : 1);
            return size;
        }
    }catch(ZMemoryReadOutOfBounds e){
        throw e;
    }
}

ObjectProperty ZObjectTable::getPropertyListElem(zword addr, ulong index) throw (IllegalPropertyIndex)
{
    // returns an ObjectProperty object for a certain property list element for a certain property list
    // addr is the pointer to the base of the property list
    // index is the property index
    ObjectProperty objProp;
    try{
        if(zVersion<=3){
            zword addr2=addr;
            for(int i=0; i<index; i++)
                addr2+=(getPropertySize(addr2)+1);
            objProp.propertyDataSize=getPropertySize(addr2);
            objProp.propertyDataAddr=addr2+1;
            return objProp;
        }else if(zVersion>3){
            bool isWordSize=false;
            zword addr2=addr;
            for(int i=0; i<index; i++)
            {
                addr2+=getPropertySize(addr2, isWordSize)+1;
                if(isWordSize) addr2+=1;
            }
            objProp.propertyDataSize=getPropertySize(addr2, isWordSize);
            objProp.propertyDataAddr=(isWordSize==1 ? (addr2+1) : (addr2+2));
        }
    }catch(...){
        THROW_ILLEGALPROPERTYINDEX(__LINE__, __FUNCTION__, __FILE__);
    }
}

zbyte ZObjectTable::getPropertyNumber(zword addr) throw (ZMemoryReadOutOfBounds){
	// returns the number of the property
	// universal for all zmachine versions
	zbyte sizeByte=zMemObjPtr->readZByte(addr);
	if(zVersion<=3){
		return (sizeByte & 31);
	}else{
		return (sizeByte & 63);
	}
}

void ZObjectTable::unlinkObject(zword index) throw (IllegalObjectIndex){
	if(!index) throw IllegalObjectIndex();
	if(zVersion<=3){
		zbyte parentObj, ySiblingObj, oSiblingObj;
		// get parent object
		parentObj=getObjectParent(index);
		if(!parentObj) return;

		// get older sibling of object
		// and set parent and sibling to 0
		setObjectParent(index, 0);
		oSiblingObj=getObjectSibling(index);
		setObjectSibling(index, 0);

		// get first child of parent (youngest sibling)
		ySiblingObj=getObjectChild(parentObj);

		// remove object from sibling list
		if(ySiblingObj==index){
			setObjectChild(parentObj, oSiblingObj);
		}else{
			zbyte currentSibling=getObjectSibling(ySiblingObj);
			while(currentSibling!=index){
				ySiblingObj=currentSibling;
				currentSibling=getObjectSibling(ySiblingObj);
			}
			setObjectSibling(ySiblingObj, oSiblingObj);
		}
	}
}