#ifndef ZOBJECT_H
#define ZOBJECT_H

#include "../../zglobal/zglobal.h"
#include "../../zerror/zerror/zerror.h"
#include "zmemory.h"

#define THROW_ILLEGALOBJECTINDEX(line, function, file)\
    throw IllegalObjectIndex((const int)line, (const char*)function, (const char*)file);

#define THROW_ILLEGALPROPERTYINDEX(line, function, file)\
    throw IllegalPropertyIndex((const int)line, (const char*)function, (const char*)file);

extern int zVersion;
extern ZError zErrorLogger;

class IllegalObjectIndex : ZException{
    public:
    IllegalObjectIndex()
    {
        zErrorLogger.addError("Error : IllegalObjectIndex thrown");
    }
    IllegalObjectIndex(const int line, const char* function, const char* file){
        zErrorLogger.addError(("Error : IllegalObjectIndex thrown at : "+compileErrorMsg(line, function, file)).c_str());
    }
};

class IllegalPropertyIndex : ZException{
    public:
    IllegalPropertyIndex()
    {
        zErrorLogger.addError("Error : IllegalPropertyIndex thrown");

    };
    IllegalPropertyIndex(const int line, const char* function, const char* file){
        zErrorLogger.addError(("Error : IllegalPropertyIndex thrown at : "+compileErrorMsg(line, function, file)).c_str());
    }
};

typedef struct ObjectProperty{
    // object property structure
    zword propertyDataAddr; // byte address of property data
    ulong propertyDataSize;  // byte size of property data
};

class ZObjectTable{
    private:
        ZMemory* zMemObjPtr;    // a pointer to the ZMemory object
                                // this is supposed to operate on
        zword zObjectTable;    // pointer to base of object table
        zword zObjectTree;     // pointer to base of object tree

        zword setObjectChildHelper(ulong indexParent, ulong indexChild) throw (IllegalObjectIndex);
        zword setObjectSiblingHelper(ulong indexObject, ulong indexSibling) throw (IllegalObjectIndex);
        zword setObjectParentHelper(ulong indexChild, ulong indexParent) throw (IllegalObjectIndex);
    public:
        ZObjectTable();
        ZObjectTable(ZMemory* zMemObj);
        ~ZObjectTable();
// todo : change ulong to zword?
        ulong getObjectAddr(ulong index) throw (IllegalObjectIndex);
        zword getDefaultProperty(ulong index) throw (IllegalPropertyIndex);
        ulong getObjectAttributeFlags32(ulong index) throw (IllegalObjectIndex);
        ulong getObjectAttributeFlags48(ulong index) throw (IllegalObjectIndex);
        zword getObjectParent(ulong index) throw (IllegalObjectIndex);
        zword getObjectSibling(ulong index) throw (IllegalObjectIndex);
        zword getObjectChild(ulong index) throw (IllegalObjectIndex);
        zword getObjectPropertyListAddr(ulong index) throw (IllegalObjectIndex);
        zword getObjectPropertyHeaderAddr(ulong index) throw (IllegalObjectIndex);
        zword* getObjectName(ulong index) throw (IllegalObjectIndex);
        zword setObjectParent(ulong indexChild, ulong indexParent) throw (IllegalObjectIndex);
        zword setObjectSibling(ulong indexObject, ulong indexSibling) throw (IllegalObjectIndex);
        zword setObjectChild(ulong indexParent, ulong indexChild) throw (IllegalObjectIndex);
		void setObjectAttributeFlags32(ulong index, ulong flags) throw (IllegalObjectIndex);
        zword getPropertyListLength(zword addr) throw (ZMemoryReadOutOfBounds);
        ObjectProperty getPropertyListElem(zword addr, ulong index) throw (IllegalPropertyIndex);
        zbyte getPropertySize(zword addr) throw (ZMemoryReadOutOfBounds);
        zword getPropertySize(zword addr, bool &isWordSizeFlag) throw (ZMemoryReadOutOfBounds);
		zbyte getPropertyNumber(zword addr) throw (ZMemoryReadOutOfBounds);
		void unlinkObject(zword index) throw (IllegalObjectIndex);
protected:
};

#endif
