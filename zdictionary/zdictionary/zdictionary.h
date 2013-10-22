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
            if(endianize(wordData[i])>>15==1){
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
        for(int i=0; endianize(a.wordData[i])>>15!=1; i++)
        {
            this->wordData[i]=a.wordData[i];
        }
        this->textBufferPos=a.textBufferPos;
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
        zbyte outData[4];
        outData[0]=(zbyte)endianize(dictWordAddr)>>8;
        outData[1]=(zbyte)endianize(dictWordAddr)&255;
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
    zbyte numWords;
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
    const ZDictionaryParseTable& operator=(const ZDictionaryParseTable& a)
    {
        this->entryVector=a.entryVector;
        this->numWords=a.numWords;
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
    ZSCIIDictionaryToken** tokenizeZSCIIString(zchar* zstring);
    ZDictionaryParseTable performLexicalAnalysis(zchar* zstring);
    zchar* getWordSeparators() {return wordSeparators;};
    bool compareWord(zword word1[2], zword word2[2]);
    bool compareWord(zword word1[3], zword word2[3], bool trash);
    protected:
};

#endif