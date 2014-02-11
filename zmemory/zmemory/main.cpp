#include <iostream>
#include <cstdio>
#include "zmemory.h"
#include "zobject.h"
#include "../../zerror/zerror/zerror.h"
#include "../../ztext/ztext/ztext.h"

using namespace std;

long filesize(FILE *stream)
{
   long curpos, length;
   curpos = ftell(stream);
   fseek(stream, 0L, SEEK_END);
   length = ftell(stream);
   fseek(stream, curpos, SEEK_SET);
   return length;
}

int zVersion;



int main()
{
    cout << "zmemory unit test!" << endl;
    FILE* storyFile=fopen("zork2.z3", "r");
    if(storyFile==NULL) return -1;
    zbyte* storyData=new zbyte[filesize(storyFile)];
    fread(storyData, filesize(storyFile), 1, storyFile);
    ZMemory zMem(storyData, filesize(storyFile));
    ZObjectTable zObj(&zMem);

    cout << "Z-machine version : " << zVersion << endl;
    cout << "Dynamic memory : "<< zMem.getZDynamicMemoryLower() << " - " << zMem.getZDynamicMemoryUpper() << endl;
    cout << "Static memory : " << zMem.getZStaticMemoryLower() << " - " << zMem.getZStaticMemoryUpper() << endl;
    cout << "High memory : " << zMem.getZHighMemoryLower() << " - " << zMem.getZHighMemoryUpper() << endl;

    cout << (int)zMem.readZByte(0) << endl;
    cout << (int)zMem.readZWord(6) << endl;

    cout << (int)zObj.getDefaultProperty(1) << endl;
    cout << (int)zObj.getObjectAttributeFlags32(224) << endl;
    cout << (int)zObj.getObjectParent(224) << ", " << (int)zObj.getObjectSibling(224) << ", " <<\
            (int)zObj.getObjectChild(224) << endl;
    cout << (int)zObj.getObjectPropertyListAddr(224) << endl;
    zObj.setObjectParent(224, 1);
    zObj.setObjectSibling(224, 2);
    zObj.setObjectChild(224, 3);
    cout << (int)zObj.getObjectParent(224) << ", " << (int)zObj.getObjectSibling(224) << ", " <<\
            (int)zObj.getObjectChild(224) << endl;
    zMem.storeZWord(0, 12345);
    cout << zMem.readZWord(0) << endl;
    int oldChild=zObj.getObjectChild(220);
    zObj.setObjectChild(221, 220);
    cout << (int)zObj.getObjectChild(221) << " & " << (int)zObj.getObjectParent(220) << endl;
    cout << (int)zObj.getObjectPropertyHeaderAddr(1) << endl;
    cout << zCharStringtoZSCII(zObj.getObjectName(247), zMem) << endl;
    cout << (int)(zObj.getPropertyListElem(zObj.getObjectPropertyListAddr(247), 0)).propertyDataSize << endl;
    for(int i=0; i<(int)zObj.getPropertyListElem(zObj.getObjectPropertyListAddr(247), 0).propertyDataSize; i++)
        printf("%X, ", (int)zMem.readZByte((zObj.getPropertyListElem(zObj.getObjectPropertyListAddr(247), 0)).propertyDataAddr+i));
    printf("\n");
    return 0;
}
