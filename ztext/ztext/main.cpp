#include <iostream>
#include "ztext.h"
#include "../../zerror/zerror/zerror.h"

ZError zErrorLogger;

using namespace std;
zchar zstring[11] = {
	0x1F, 0x51, 0x31, 0xD3, 0x30, 0x08, 0x35, 0x58, 0xE4, 0xA5, 0x79
};

int main()
{
    cout << "Z-Text unit test!" << endl;
    zword* zstr_w=(zword*)zstring;
    for(int i=0; i<sizeof(zstring)/2; i++)
    {
        zstr_w[i]=endianize(zstr_w[i]);
    }
    cout << zCharStringtoZSCII(zstr_w) << endl;
    cout << ZSCIIGetResidentAlphabet('-') << endl;
    cout << (int)getZCharAlphaShiftCharacter(0, 2) << endl;
    int a=0;
    zword out;
    cout << (out=ZSCIItoZChar((zchar*)"Grue gets you ", a)) << endl;
    cout << zChartoZSCII(out) << endl;
    cout << zCharStringtoZSCII(ZSCIItoZCharString((zchar*)"Hello World! This is me!      ")) << endl;;
    return 0;
}
