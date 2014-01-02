#include <iostream>
#include "zstack.h"
#include "../../zglobal/zglobal.h"

using namespace std;

ZError zErrorLogger;

int main()
{
    ZStack z;
    cout << "Hello world!" << endl;
    z.initStack(1024);
    cout << "Initialized stack" << endl;
    cout << z.isStackEmpty() << endl;
    z.push(100);
    cout << z.pull() << endl;
    for(int i=0; i<1024; i++)
        z.push(i);
    for(int i=0; i<1024; i++)
        cout << z.pull() << endl;
    z.pull();
    return 0;
}
