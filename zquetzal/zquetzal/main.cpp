#include <iostream>
#include <cstdlib>
#include "../../zmemory/zmemory/zmemory.h"
#include "zquetzal.h"
#include "../../zglobal/zglobaldefines.h"

#if defined(PLATFORM_LINUX_CONSOLE)
#include <curses.h>
#endif

int zVersion=3;

using namespace std;

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
	#if defined PLATFORM_LINUX_CONSOLE
	initscr();
	echo();
	keypad(stdscr, true);
	scrollok(stdscr, true);
	printw("Zebra Z-Machine Interpreter v1.0 alpha\n");
	printw("Code licensed under GPL v3 by Stephen Zhang\n");
	printw("You are running the Linux version, powered by NCURSES\n\n");
	refresh();
	#endif
    FILE* storyFile=fopen("zork2.z3", "r");
    if(storyFile==NULL) return -1;
    zbyte* storyData=new zbyte[filesize(storyFile)];
    fread(storyData, filesize(storyFile), 1, storyFile);
    ZMemory zMem(storyData, filesize(storyFile));
	ZObjectTable zObj(&zMem);
	ZStack zStack;
	ZInOut zInOut;
	ZDictionary zDict(&zMem);
	ZQuetzalSave q(zMem, zStack);
	#ifdef PLATFORM_LINUX_CONSOLE
	getch();
	endwin();
	#endif
    return 0;
}
