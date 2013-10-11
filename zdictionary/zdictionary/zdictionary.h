#ifndef ZDICTIONARY_H
#define ZDICTIONARY_H

#include <exception>
#include "../../zglobal/zglobal.h"
#include "../../zmemory/zmemory/zmemory.h"
#include "../../ztext/ztext/ztext.h"

class IllegalDictionaryIndex : std::exception{
	public:
	IllegalDictionaryIndex(){
		zErrorLogger.addError("Error : IllegalDictionaryIndex thrown");
	}
};

struct zDictionaryWord{
	zchar* wordData;
	// assignment operators necessary for using STL vector
	zDictionaryWord(){}
	zDictionaryWord(zchar* wordData)
	{
		this->wordData=new zchar[ZSCIIStrLen(wordData)];
		this->wordData[0]=NULL;
		ZSCIIStrCat(this->wordData, wordData);
	}
	const zDictionaryWord& operator=(const zDictionaryWord &a)
	{
		this->wordData=a.wordData;
		return *this;
	}
};

class ZDictionary{
    private:
    ZMemory* zMemObjPtr;
    zword zDictionaryAddr;
    zword zDictionaryEntriesBase;
    // note : it is legal for the interpreter
    // to copy the word separators to another memory loc
    // because the dictionary resides in static memory
    // therefore the game cannot alter it.
    zchar* wordSeparators;
    zbyte entryLength;
    zword numOfEntries;
    bool isWordSeparator(zchar zscii);
    void makeLowerCase(zchar* zstring);

    public:
    ZDictionary();
    ZDictionary(ZMemory *zMemObj) throw (ZMemoryReadOutOfBounds);
    ~ZDictionary();
    zword getDictionaryEntryAddr(ulong index) throw (IllegalDictionaryIndex);
    zDictionaryWord** tokenizeZSCIIString(zchar* zstring);
    zchar* getWordSeparators() {return wordSeparators;};
    protected:
};

#endif
