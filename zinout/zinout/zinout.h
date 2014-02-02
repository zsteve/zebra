#ifndef ZINOUT_H
#define ZINOUT_H

#include "../../zglobal/zglobaldefines.h"
#include <iostream>
#include <cstdlib>
#include <cstdio>

#ifdef PLATFORM_WIN32_CONSOLE
#include <conio.h>
#elif defined PLATFORM_LINUX_CONSOLE
#include <ncurses.h>
#endif

using namespace std;

class ZInOut{
    public:
#ifdef PLATFORM_LINUX_CONSOLE
	enum color{
		BLACK=COLOR_BLACK,
		RED=COLOR_RED,
		GREEN=COLOR_GREEN,
		YELLOW=COLOR_YELLOW,
		BLUE=COLOR_BLUE,
		MAGENTA=COLOR_MAGENTA,
		CYAN=COLOR_CYAN,
		WHITE=COLOR_WHITE
	};
#endif
    ZInOut();
    void printLine(char* str);
    void print(char* str);
    void readLine(char* buffer);
    char* readLine();
    void read(char* buffer);
    char* read();

    void setTextStyle(int style);
    void clearScreen();
    void clearLine();
    void setCursorPos(int line, int col);
    int getCursorX();
    int getCursorY();
    void saveCursorPos();
    void restoreCursorPos();
    char getChar();
#ifdef PLATFORM_LINUX_CONSOLE
    void setTextColor(int pair, color fg, color bg);
#endif
    private:

    static const char symchars[];
    static const char ctrlchars[];
    bool issym(int c);
    bool isctrl(int c);

	int curx, cury;

    protected:
};

#endif
