#include "zdictionary.h"
#include <vector>
#include <string>
#include <cctype>

using namespace std;

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
		zDictionaryEntriesBase=(zDictionaryAddr+size+4);
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
		THROW_ILLEGALDICTIONARYINDEX(__LINE__, __FUNCTION__, __FILE__)
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

bool ZDictionary::compareWord(zword word1[2], zword word2[2])
{
    for(int i=0; i<2; i++)
    {
        if(word1[i]!=word2[i])
         return false;
    }
    return true;
}

bool ZDictionary::compareWord(zword word1[3], zword word2[3], bool trash)
{
    for(int i=0; i<6; i++)
    {
        if(word1[i]!=word2[i])
            return false;
    }
    return true;
}

vector<ZSCIIDictionaryToken> ZDictionary::tokenizeZSCIIString(zchar* zstring)
{
	// tokenizes a ZSCII string into an array of ZSCIIDictionaryTokens
	// first we must process the string
	int len=ZSCIIStrLen(zstring);
	makeLowerCase(zstring);
	std::vector<ZSCIIDictionaryToken> tokenVector(0);	// vector for our tokens
	string tempToken("");
	int wordBeginPos=1;
	for(int i=0; ; i++)
	{
		if(zstring[i]==' '){	// space is always regarded as word separator
													// and is always ignored (not taken as a word in itself)
			if(!tempToken.empty()){	// if not empty
				tokenVector.push_back(ZSCIIDictionaryToken((zchar*)tempToken.c_str(), wordBeginPos));
				tempToken="";
				wordBeginPos=i+1;
			}else{
				// else, its a trailing space and we ignore it
				wordBeginPos++;
			}
		}else if(isWordSeparator(zstring[i])){
			// if it's a word separator, it is treated as a word by itself
			if(!tempToken.empty())
			{
				tokenVector.push_back(ZSCIIDictionaryToken((zchar*)tempToken.c_str(), wordBeginPos));
				tempToken="";
				wordBeginPos=i;
			}
			tempToken+=zstring[i];
			tempToken+="";
			tokenVector.push_back(ZSCIIDictionaryToken((zchar*)tempToken.c_str(), wordBeginPos));
			tempToken="";
			wordBeginPos=i+1;
		}else if(zstring[i]!=NULL){
				tempToken+=zstring[i];
		}else if(zstring[i]==NULL){
			// null terminator char.
			if(!tempToken.empty())
			{
				tokenVector.push_back(ZSCIIDictionaryToken((zchar*)tempToken.c_str(), wordBeginPos));
				tempToken="";
			}
			break;
		}
	}
	return tokenVector;
}

/** performs lexical analysis on a ZSCII string
 * @param zstring pointer to string
 */
ZDictionaryParseTable ZDictionary::performLexicalAnalysis(zchar* zstring)
{
    ZDictionaryParseTable parseTable;
    vector<ZSCIIDictionaryToken> tokens=tokenizeZSCIIString(zstring);
    std::vector<ZCharDictionaryToken> zCharTokenVector(0); /** vector for converted zchar tokens */
    for(int i=0; i<tokens.size(); i++)
    {
        zword* zcharString=ZSCIItoZCharString(tokens[i].wordData); /** converted zchars */

        zword* zcharDictionaryToken=zChartoDictionaryZCharString(zcharString);
        zCharTokenVector.push_back(ZCharDictionaryToken(zcharDictionaryToken, tokens[i].textBufferPos));
        delete[] zcharString;
        delete[] zcharDictionaryToken;
    }
    // now we need to look these up in the dictionary
    for(int i=0; i<zCharTokenVector.size(); i++)
    {
        zword* wordData;
        if(zVersion<=3)
        {
            wordData=new zword[2];
            wordData[0]=zCharTokenVector[i].wordData[0];
            wordData[1]=zCharTokenVector[i].wordData[1];
        }else if(zVersion>3){
            wordData=new zword[3];
            wordData[0]=zCharTokenVector[i].wordData[0];
            wordData[1]=zCharTokenVector[i].wordData[1];
            wordData[2]=zCharTokenVector[i].wordData[2];
        }
        bool matchFound=false;
        for(int j=0; j<numOfEntries; j++)
        {
            if(zVersion<=3)
            {
                if(compareWord(&wordData[0], endianizeString((zword*)(zMemObjPtr->getRawDataPtr()+getDictionaryEntryAddr(j))))==true){
                	// if we found the right one, add entry to parse table
               		parseTable.addEntry(getDictionaryEntryAddr(j), ZSCIIStrLen(tokens[i].wordData), (tokens[i].textBufferPos));
               		matchFound=true;
               		break;
                }
            }else if(zVersion>3){
                if(compareWord(&wordData[0], endianizeString((zword*)(zMemObjPtr->getRawDataPtr()+getDictionaryEntryAddr(j)))), false){ /** false given just so we can differentiate
                                                                                                                                                                                                                                    between two types of compareWord() */
                    // if we found the right one, add entry to parse table
                    parseTable.addEntry(getDictionaryEntryAddr(j), ZSCIIStrLen(tokens[i].wordData), (tokens[i].textBufferPos));
                    matchFound=true;
                    break;
                }
            }
        }
        if(!matchFound){
            parseTable.addEntry(NULL, ZSCIIStrLen(tokens[i].wordData), tokens[i].textBufferPos);
        }
    }
    return parseTable;
}
