#ifndef ZOBJECT_H
#define ZOBJECT_H

#include "../../zglobal/zglobal.h"
#include "../../zerror/zerror/zerror.h"
#include "zmemory.h"

extern int zVersion;
extern ZError zErrorLogger;

class IllegalObjectIndex : std::exception{
    public:
    IllegalObjectIndex()
    {
        zErrorLogger.addError("Error : IllegalObjectIndex thrown");
    }
};

class IllegalPropertyIndex : std::exception{
    public:
    IllegalPropertyIndex()
    {
        zErrorLogger.addError("Error : IllegalPropertyIndex thrown");
    }
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
        zword getPropertyListLengthHelper(zword addr) throw (ZMemoryReadOutOfBounds);
        zword getPropertyListLengthHelper(zword addr, bool &isWordSizeFlag) throw (ZMemoryReadOutOfBounds);

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
    protected:
};

#endif
