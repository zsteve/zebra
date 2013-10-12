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

    cout << zCharStringtoZSCII(ZSCIItoZCharString((zchar*)"floating-point"), zMem) << endl;
    cout << zCharStringtoZSCII(zChartoDictionaryZCharString(ZSCIItoZCharString((zchar*)"floating-point")), zMem) << endl;

    cout << "Tokenizer test : type \"quit\" to exit" << endl;
    for(;;)
    {
        string inString;
        getline(cin, inString);
        if(inString=="quit") break;
        zDictionaryWord** tkns=zDict.tokenizeZSCIIString((zchar*)inString.c_str());
        cout << endl << "Tokens:" << endl << endl;
        for(int i=0; tkns[i]!=NULL; i++)
            cout << "[" << tkns[i]->wordData << "]" << endl;
        cout << endl;
    }
    return 0;
}
