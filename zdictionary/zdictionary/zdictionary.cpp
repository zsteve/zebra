#include "zdictionary.h"
#include <vector>
#include <string>
#include <cctype>

ZDictionary::ZDictionary()
{
	zMemObjPtr=NULL;
}

ZDictionary::ZDictionary(ZMemory *zMemObj) throw (ZMemoryReadOutOfBounds)
{
	zMemObjPtr=zMemObj;
	try{
		zDictionaryAddr=zMemObjPtr->readZWord(0x8);
		ulong size=zMemObjPtr->readZByte(zDictionaryAddr);
		wordSeparators=new zchar[size+1];
		for(int i=0; i<size; i++)
		{
			wordSeparators[i]=zMemObjPtr->readZByte(zDictionaryAddr+1+i);	// read 1 char of word separators list
		}
		wordSeparators[size]=NULL;
		entryLength=zMemObjPtr->readZByte(zDictionaryAddr+size+1);
		numOfEntries=zMemObjPtr->readZWord(zDictionaryAddr+size+2);
		zDictionaryEntriesBase=(zDictionaryAddr+size+3);
	}catch(ZMemoryReadOutOfBounds e){
		throw e;
	}
}

ZDictionary::~ZDictionary()
{
	delete[] wordSeparators;
}

zword ZDictionary::getDictionaryEntryAddr(ulong index) throw (IllegalDictionaryIndex)
{
	if(index>numOfEntries)
	{
		throw IllegalDictionaryIndex();
	}
	return (zDictionaryEntriesBase+(index*entryLength));
}

bool ZDictionary::isWordSeparator(zchar zscii)
{
	int len=ZSCIIStrLen(wordSeparators);
	for(int i=0; i<len; i++)
	{
		if(wordSeparators[i]==zscii) return true;
	}
	return false;
}

void ZDictionary::makeLowerCase(zchar* zstring)
{
	// makes an entire ZSCII string lower case
	if(!zstring) return;
	int len=ZSCIIStrLen(zstring);
	for(int i=0; i<len; i++)
	{
		if(isupper(zstring[i])){
			zstring[i]=tolower(zstring[i]);
		}
	}
}

zDictionaryWord** ZDictionary::tokenizeZSCIIString(zchar* zstring)
{
	// tokenizes a ZSCII string into an array of zDictionaryWords
	// first we must process the string
	int len=ZSCIIStrLen(zstring);
	makeLowerCase(zstring);
	std::vector<zDictionaryWord*> tokenVector(0);	// vector for our tokens
	string tempToken("");
	for(int i=0; ; i++)
	{
		if(zstring[i]==' '){	// space is always regarded as word separator
													// and is always ignored (not taken as a word in itself)
			if(!tempToken.empty()){	// if not empty
				tokenVector.push_back(new zDictionaryWord((zchar*)tempToken.c_str()));
				tempToken="";
			}else{
				// else, its a trailing space and we ignore it
			}
		}else if(isWordSeparator(zstring[i])){
			// if it's a word separator, it is treated as a word by itself
			if(!tempToken.empty())
			{
				tokenVector.push_back(new zDictionaryWord((zchar*)tempToken.c_str()));
				tempToken="";
			}
			tempToken+=zstring[i];
			tempToken+="";
			tokenVector.push_back(new zDictionaryWord((zchar*)tempToken.c_str()));
			tempToken="";
		}else if(zstring[i]!=NULL){
				tempToken+=zstring[i];
		}else if(zstring[i]==NULL){
			// null terminator char.
			if(!tempToken.empty())
			{
				tokenVector.push_back(new zDictionaryWord((zchar*)tempToken.c_str()));
				tempToken="";
			}
			tokenVector.push_back(NULL); // NULL pointer terminates pointer array
			break;
		}
	}
	zDictionaryWord** outArray=new zDictionaryWord*[tokenVector.size()];
	for(int i=0; i<tokenVector.size(); i++)
	{
		outArray[i]=tokenVector[i];
	}
	return outArray;
}
