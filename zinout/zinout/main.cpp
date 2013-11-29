#include <iostream>
#include "zinout.h"

using namespace std;

ZInOut zIO;

int main()
{
    cout << "zinout test!" << endl;
    zIO.setTextStyle(zIO.BGLIGHTGRAY);
    zIO.setTextStyle(zIO.FGBLUE);
    zIO.clearScreen();
    cout << "hello world in blue" << endl;
    cout << "blah";
    zIO.setCursorPos(0, 0);
    zIO.clearLine();
    zIO.print("hello! I'm zinout!");
    zIO.setCursorPos(0, 0);
    zIO.clearLine();
    zIO.setTextStyle(zIO.BGRED);
    cout << "\n\n\n\n\n" << endl;
    char* buf = new char[80];
    zIO.readLine(buf);
    return 0;
}
