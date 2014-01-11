#include "zinout.h"
#include <iostream>
#include <cstdlib>
#include <cctype>

#ifdef PLATFORM_WIN32_CONSOLE
#include <conio.h>
#elif defined (PLATFORM_LINUX_CONSOLE)
#include <curses.h>
#endif

// symbolic chars
const char ZInOut::symchars[]={"!@#$%^&*()_+|/\\?<>~"};
const char ZInOut::ctrlchars[]={0x0D, 0x0A, 0x00};

bool ZInOut::issym(int c){
	// is character symbolic
	for(int i=0; symchars[i]!=NULL; i++){
		if(symchars[i]==c) return true;
	}
	return false;
}

bool ZInOut::isctrl(int c){
	// is character control (ENTER)
	for(int i=0; ctrlchars[i]!=NULL; i++){
		if(ctrlchars[i]==c) return true;
	}
	return false;
}

ZInOut::ZInOut(){
}

void ZInOut::print(char* str){
    #if defined (PLATFORM_WIN32_CONSOLE)
    cout << str;
    #elif defined (PLATFORM_LINUX_CONSOLE)
	printw("%s", str);
	refresh();
    #endif
}

void ZInOut::printLine(char* str){
	print(str);
	print("\n");
}

void ZInOut::readLine(char* buffer){
	#if defined(PLATFORM_WIN32_CONSOLE)
    scanf("%s", buffer);
	#elif defined(PLATFORM_LINUX_CONSOLE)
	getstr(buffer);
	#endif
}

char* ZInOut::readLine(){
    #if defined(PLATFORM_WIN32_CONSOLE)
    static char* buffer=NULL;
    if(buffer){
        delete[] buffer;
    }
    buffer=new char[80];
    gets(buffer);
    return buffer;
    #elif defined(PLATFORM_LINUX_CONSOLE)
    static char* buffer=NULL;
    if(buffer){
		delete[] buffer;
    }
    buffer=new char[80];
	getstr(buffer);
	return buffer;
    #endif
}

char ZInOut::getChar(){
	#ifdef PLATFORM_WIN32_CONSOLE
	return _getch();
	#elif defined PLATFORM_LINUX_CONSOLE
	return getch();
	#endif
}

void ZInOut::setCursorPos(int x, int y){
	#if defined PLATFORM_LINUX_CONSOLE
	move(y, x);
	#endif
}

int ZInOut::getCursorX(){
	#if defined PLATFORM_LINUX_CONSOLE
	return getcurx(stdscr);
	#endif
}

int ZInOut::getCursorY(){
	#if defined PLATFORM_LINUX_CONSOLE
	return getcury(stdscr);
	#endif
}

void ZInOut::saveCursorPos(){
	#if defined PLATFORM_LINUX_CONSOLE
	curx=getCursorX();
	cury=getCursorY();
	#endif
}

void ZInOut::restoreCursorPos(){
	#if defined PLATFORM_LINUX_CONSOLE
	setCursorPos(curx, cury);
	#endif
}

void ZInOut::setTextColor(color fg, color bg){
	#if defined PLATFORM_LINUX_CONSOLE
	start_color();
	init_pair(1, (int)fg, (int)bg);
	attron(COLOR_PAIR(1));
	#endif
}

