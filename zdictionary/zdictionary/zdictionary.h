#ifndef ZDICTIONARY_H
#define ZDICTIONARY_H

#include <cstdio>
#include <string>
#include <exception>
#include "../../zglobal/zglobal.h"
#include "../../zmemory/zmemory/zmemory.h"
#include "../../ztext/ztext/ztext.h"

using namespace std;

// Exception throwing macros
#define THROW_ILLEGALDICTIONARYINDEX(line, function, file)\
    throw IllegalDictionaryIndex((int)line, (char*)function, (char*)file);

#define THROW_DICTIONARYERROR(line, function, file)\
    throw DictionaryError((int)line, (char*)function, (char*)file);

class IllegalDictionaryIndex : ZException{
	public:
	IllegalDictionaryIndex(){
		zErrorLogger.addError("Error : IllegalDictionaryIndex thrown");
	}
    IllegalDictionaryIndex(const int line, const char* function, const char* file){
        zErrorLogger.addError(("Error : IllegalDictionaryIndex thrown : "+compileErrorMsg(line, function, file)).c_str());
        return;
	}
};

class DictionaryError : ZException{
    public:
    DictionaryError(){
        zErrorLogger.addError("Error : DictionaryError thrown");
    }
    DictionaryError(const int line, const char* function, const char* file){
        zErrorLogger.addError(("Error : DictionaryError thrown : "+compileErrorMsg(line, function, file)).c_str());
    }
};

struct ZSCIIDictionaryToken{
	zchar* wordData;
	int textBufferPos;
	// assignment operators necessary for using STL vector
	ZSCIIDictionaryToken(){wordData=NULL; textBufferPos=NULL;}
	ZSCIIDictionaryToken(zchar* wordData, int textBufferPos)
	{
		this->wordData=new zchar[ZSCIIStrLen(wordData)];
		this->wordData[0]=NULL;
		ZSCIIStrCat(this->wordData, wordData);
		this->textBufferPos=textBufferPos;
	}
	const ZSCIIDictionaryToken& operator=(const ZSCIIDictionaryToken &a)
	{
	    delete[] this->wordData;
	    this->wordData=new zchar[ZSCIIStrLen(a.wordData)];
		ZSCIIStrCpy(a.wordData, this->wordData);
		this->textBufferPos=a.textBufferPos;
		return *this;
	}
};

struct ZCharDictionaryToken{
    zword* wordData;
    int textBufferPos;
    ZCharDictionaryToken(){wordData=NULL; textBufferPos=NULL;}
    ZCharDictionaryToken(zword* wordData, int textBufferPos)
    {
        this->wordData=new zword[zCharStrLen(wordData)];
        for(int i=0; ; i++)
        {
            this->wordData[i]=wordData[i];
            if((wordData[i])>>15==1){
                break;
            }
        }
        this->textBufferPos=textBufferPos;
        return;
    }
    const ZCharDictionaryToken& operator=(const ZCharDictionaryToken &a)
    {
        delete[] wordData;
        this->wordData=new zword[zCharStrLen(a.wordData)];
        for(int i=0; (a.wordData[i])>>15!=1; i++)
        {
            this->wordData[i]=a.wordData[i];
        }
        this->textBufferPos=a.textBufferPos;
		return *this;
    }
};

/** single entry of ZDictionaryParseTable
*/

struct ZDictionaryParseTableEntry{
    /*
    Each block consists of the byte address of the word in the dictionary, if it is
    in the dictionary, or 0 if it isn't; followed by a byte giving the number of
    letters in the word; and finally a byte giving the position in the text-buffer
    of the first letter of the word. */
    zword dictWordAddr;
    zbyte letterCount;
    zbyte textBufferPos;
    ZDictionaryParseTableEntry(){
        dictWordAddr=NULL;
        letterCount=NULL;
        textBufferPos=NULL;
    }
    ZDictionaryParseTableEntry(zword dictWordAddr, zbyte letterCount, zbyte textBufferPos)
    {
        this->dictWordAddr=dictWordAddr;
        this->letterCount=letterCount;
        this->textBufferPos=textBufferPos;
    }
    ~ZDictionaryParseTableEntry(){}
    const ZDictionaryParseTableEntry& operator=(const ZDictionaryParseTableEntry& a)
    {
        this->dictWordAddr=a.dictWordAddr;
        this->letterCount=a.letterCount;
        this->textBufferPos=a.textBufferPos;
        return *this;
    }
    /** returns a packed (4 zbytes) array for the entry
     * the zword value (zbytes 0, 1) is big endianized
     */
    zbyte* getPackedEntry(){
        zbyte* outData=new zbyte[4];
        outData[0]=(zbyte)((dictWordAddr)>>8);
        outData[1]=(zbyte)((dictWordAddr)&255);
        outData[2]=letterCount;
        outData[3]=textBufferPos;
        return outData;
    }
};

/** intermediate structure for dictionary parse table
*/
struct ZDictionaryParseTable{
    /* byte 0 of the parse-buffer should hold the maximum number of textual words which
    can be parsed. (If this is n, the buffer must be at least 2 + 4*n bytes long to
    hold the results of the analysis.) */
    std::vector<ZDictionaryParseTableEntry> entryVector; /** vector for entries */
    ZDictionaryParseTable() : entryVector(0){
    }
    ~ZDictionaryParseTable(){
    }
    /** constructs and adds a ZDictionaryParseTableEntry to the parse table
     * @param dictWordAddr byte address of word in dictionary table
     * @param letterCount number of letters in dictionary word
     * @param textBufferPos byte position of first letter of word in text buffer
     */
    void addEntry(zword dictWordAddr, zbyte letterCount, zbyte textBufferPos)
    {
        entryVector.push_back(ZDictionaryParseTableEntry(dictWordAddr, letterCount, textBufferPos));
    }

    /** adds a ZDictionaryParseTableEntry to the parse table
     * @param e new entry
     */
    void addEntry(ZDictionaryParseTableEntry e)
    {
        entryVector.push_back(e);
    }

    /** trims the parse table to a certain number of entries
     * @param maxNumWords max number of words
     */
    void trimTable(int maxNumWords)
    {
        std::vector<ZDictionaryParseTableEntry>::iterator it=entryVector.begin();
        int i=0;
        if(entryVector.size()<=maxNumWords)
        {
            return;
        }
        for(it; it!=entryVector.end(); ++it)
        {
            i++;
            if(i>maxNumWords){
                // if we have stepped to the last allowed word
                // all subsequent words are to be discarded
                entryVector.erase(it, entryVector.end());
                return;
            }
        }
        return;
    }
    void writeParseBuffer(ulong addr, ZMemory& zMem, bool writeUnrecognized=true) throw (DictionaryError,\
                                                            ZMemoryReadOutOfBounds,
                                                            ZMemoryWriteOutOfBounds) {
        // byte 0 of parse buffer contains max num of words buffer can hold
        zbyte maxWords=zMem.readZByte(addr+0);
        // Interpreter is asked to halt if the parse buffer has length of less than 6 bytes
        if( ((maxWords*4) - 2) <6){
            THROW_DICTIONARYERROR(__LINE__, __FUNCTION__, __FILE__);
        }
        if(entryVector.size()>255){
            trimTable(255); // if vector contains more than 255 words, trim it to 255 words
        }
        zbyte numWords=(zbyte)entryVector.size();
        if(numWords>maxWords){
            trimTable(maxWords);
            numWords=maxWords;
        }
        // byte 1 of parse buffer contains the written number of words
		if(writeUnrecognized){
		   zMem.storeZByte(addr+1, numWords);
		}else{
			int num=0;
			for(int i=0; i<entryVector.size(); i++){
				if(entryVector[i].dictWordAddr){
					num++;
				}
			}
			zMem.storeZByte(addr+1, num);		// manually determine word count
		}
        // from byte 2 onwards, each 4 byte block is one entry
        // (of 4 byte block)
        // byte 0, 1    : byte addr of word in dictionary, 0 if not present
        // byte 2       : number of letters in word
        // byte 3       : index of first letter of word in text buffer
        for(int i=0; i<entryVector.size(); i++)
        {
			if(!entryVector[i].dictWordAddr && !writeUnrecognized) continue;	// don't write ones which aren't in dictionary IF flag set
            zbyte* zData=entryVector[i].getPackedEntry();
            for(int j=0; j<4; j++){
                zMem.storeZByte(addr+(4*i)+2+j, zData[j]);
            }
			delete[] zData;
        }
    }
    const ZDictionaryParseTable& operator=(const ZDictionaryParseTable& a)
    {
        this->entryVector=a.entryVector;
        return *this;
    }
};

class ZDictionary{
    private:
    ZMemory* zMemObjPtr;
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
    zword zDictionaryAddr;
    zword zDictionaryEntriesBase;

    ZDictionary();
    ZDictionary(ZMemory *zMemObj) throw (ZMemoryReadOutOfBounds);
    ~ZDictionary();
    zword getDictionaryEntryAddr(ulong index) throw (IllegalDictionaryIndex);
    vector<ZSCIIDictionaryToken> tokenizeZSCIIString(zchar* zstring);
    ZDictionaryParseTable performLexicalAnalysis(zchar* zstring);
    zchar* getWordSeparators() {return wordSeparators;};
    int getNumOfEntries(){return numOfEntries;}
    bool compareWord(zword word1[2], zword word2[2]);
    bool compareWord(zword word1[3], zword word2[3], bool trash);
    protected:
};

#endif
