#include <iostream>
#include "zinout.h"

using namespace std;

ZInOut zIO;

int main()
{
    cout << "zinout test!" << endl;
    zIO.setTextStyle(zIO.BGLIGHTGRAY);
    zIO.setTextStyle(zIO.FGBLACK);
    zIO.clearScreen();
    cout << "hello world in blue" << endl;
    cout << "blah";
    zIO.moveCursorPos(0, -100);
    zIO.clearLine();
    cout << "\n\n\n\n\n" << endl;
    return 0;
}
