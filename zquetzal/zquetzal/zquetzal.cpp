#include "zquetzal.h"
#include "../../zglobal/zglobal.h"
#include <cstdio>
#include <cstring>

#include "../../zstack/zstack/zstack.h"
#include "../../zmemory/zmemory/zobject.h"
#include "../../ztext/ztext/ztext.h"
#include "../../zinout/zinout/zinout.h"
#include "../../zdictionary/zdictionary/zdictionary.h"

const char formChunks[8][5]={
	"IFhd",
	"CMem",
	"UMem",
	"Stks",
	"IntD",
	"AUTH",
	"(c) ",
	"ANNO"
};

ZQuetzalSave::ZQuetzalSave(ZMemory& zMem, ZStack& zStack) : zMem(zMem), zStack(zStack){
	// read dynamic memory
	dynMemSize=zMem.getZDynamicMemoryUpper()-zMem.getZDynamicMemoryLower()+1;
	dynMem=new zbyte[dynMemSize];
	for(int i=zMem.getZDynamicMemoryLower(); i<=zMem.getZDynamicMemoryUpper(); i++){
		// do bytewise copy
		dynMem[i]=zMem.readZByte(i);
	}
	// read stack memory
	stackMemSize=zStack.getStackSize()*2;		// zStack::stackSize is in zwords
	stackMem=new zbyte[stackMemSize];
	for(int i=0; i<stackMemSize; i++){
		stackMem[i]=((zbyte*)zStack.getStackData())[i];
	}
}

ZQuetzalSave::~ZQuetzalSave(){
}

ZQuetzalSave::formID ZQuetzalSave::getFormID(char* formChunk){
	/** doesn't care if formChunk is not NULL terminated **/
	char chunkID[5];
	for(int i=0; i<4; i++) chunkID[i]=formChunk[i];
	chunkID[5]=NULL;
	for(int i=0; i<sizeof(formChunks)/5; i++){
		if(!strcmp(&formChunks[i][0], chunkID)) return (ZQuetzalSave::formID)i;
	}
	throw ZException();
}

zbyte* ZQuetzalSave::compressData(zbyte* data, zbyte* dataOrig, ulong size, int* outSize){
	zbyte* dataXOR=new zbyte[size];
	zbyte* dataOut=new zbyte[size];

	for(int i=0; i<size; i++){
		dataXOR[i]=data[i]^dataOrig[i];
	}

	for(int i=0, j=0; i<size; i++){
		if(dataXOR[i]){
			// byte was changed since last time, write the byte
			dataOut[j++]=dataXOR[i];
		}else{
			// byte unchanged, scan for number of unchanged bytes
			// write [0, n], representing n+1 unchanged bytes
			int n=1;
			for(i++; i<size && !dataXOR[i]; i++);
			dataOut[j++]=0;
			dataOut[j++]=n;
		}
	}
}

zbyte* ZQuetzalSave::writeSaveGame(){
	// writes a save game

}
