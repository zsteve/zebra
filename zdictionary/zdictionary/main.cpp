#include <iostream>
#include <cstdio>
#include "zdictionary.h"
#include "../../zglobal/zglobal.h"
#include "../../ztext/ztext/ztext.h"
#include "../../zmemory/zmemory/zmemory.h"
#include "../../zmemory/zmemory/zobject.h"
#include "../../zerror/zerror/zerror.h"

using namespace std;

int zVersion;

long filesize(FILE *stream)
{
   long curpos, length;
   curpos = ftell(stream);
   fseek(stream, 0L, SEEK_END);
   length = ftell(stream);
   fseek(stream, curpos, SEEK_SET);
   return length;
}

int main()
{
    cout << "ZDictionary unit test!" << endl;
    FILE* storyFile=fopen("zork2.z3", "r");
    if(storyFile==NULL) return -1;
    zbyte* storyData=new zbyte[filesize(storyFile)];
    fread(storyData, filesize(storyFile), 1, storyFile);
    ZMemory zMem(storyData, filesize(storyFile));
    ZObjectTable zObj(&zMem);

    ZDictionary zDict(&zMem);

    cout << "Word separators : " << zDict.getWordSeparators() << endl;

    cout << "Address of dictionary header : " << zDict.zDictionaryAddr << endl;
    cout << "Address of dictionary table : " << zDict.zDictionaryEntriesBase << endl;

    cout << zCharStringtoZSCII(ZSCIItoZCharString((zchar*)"floating-point"), zMem) << endl;
    cout << zCharStringtoZSCII(zChartoDictionaryZCharString(ZSCIItoZCharString((zchar*)"floating-point")), zMem) << endl;

    for(int i=0; i<zDict.getNumOfEntries(); i++)
        cout << zCharStringtoZSCII(dictionaryZCharStringtoZCharString(zDict.getDictionaryEntryAddr(i), zMem), zMem) << endl;

    cout << zCharStringtoZSCII(dictionaryZCharStringtoZCharString(zChartoDictionaryZCharString(ZSCIItoZCharString((zchar*)"dictionaryword"))), zMem) << endl;
    cout << "Tokenizer test : type \"quit\" to exit" << endl;
    for(;;)
    {
        string inString;
        getline(cin, inString);
        if(inString=="quit") break;
        vector<ZSCIIDictionaryToken> tkns=zDict.tokenizeZSCIIString((zchar*)inString.c_str());
        cout << endl << "Tokens:" << endl << endl;
        for(int i=0; i<tkns.size(); i++)
            cout << "[" << tkns[i].wordData << "]" << tkns[i].textBufferPos << endl;
        ZDictionaryParseTable tbl=zDict.performLexicalAnalysis((zchar*)inString.c_str());
        zMem.storeZByte(0, 59);
        tbl.writeParseBuffer(0, zMem);
        cout << "Value : " <<  (int)zMem.readZByte(1) << endl;
        for(int i=0; i<tbl.entryVector.size(); i++)
        {
            cout << (int)tbl.entryVector[i].dictWordAddr << " "\
                 << (int)tbl.entryVector[i].letterCount << " "\
                 << (int)tbl.entryVector[i].textBufferPos << " " << endl;
        }
        cout << endl;
    }
    return 0;

}
