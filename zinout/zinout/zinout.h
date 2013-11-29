#ifndef ZINOUT_H
#define ZINOUT_H

#include "../../zglobal/zglobaldefines.h"
#include <iostream>
#include <cstdlib>
#include <cstdio>

using namespace std;

class ZInOutBase{
    public:
    ZInOutBase(){}
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
    private:
    protected:
};

#ifdef PLATFORM_LINUX_CONSOLE
class ZInOut : public ZInOutBase{
    public:
    enum consoleStyle{NORMAL=0, BOLD, UNDERLINE=4, BLINK_ON, REVERSE_VIDEO_ON=7, NONDISPLAYED,\
                        FGBLACK=30, FGRED, FGGREEN, FGBROWN, FGBLUE, FGMAGENTA, FGCYAN, FGLIGHTGRAY,\
                        BGBLACK=40, BGRED, BGGREEN, BGBROWN, BGBLUE, BGMAGENTA, BGCYAN, BGLIGHTGRAY};

    enum consoleScreenMode{SM_40x25_BW, SM_40x25_CLR, SM_80x25_BW, SM_80x25_CLR, SM_320x200_CLR_GFX,\
    					SM_320x200_BW_GFX, SM_640x200_BW_GFX, SM_WRAP_END_LINE};

    ZInOut(){
        readLineLastRead=NULL;
    }
    void printLine(char* str);
    void print(char* str);
    void readLine(char* buffer);
    char* readLine();

    void setTextStyle(consoleStyle style){
        char styleCmd[20];
        snprintf(styleCmd, 20, "\033[%dm", style);
        print(styleCmd);
    }

    void clearScreen(){
        print("\033[2J");
        print("\033[0;0H");
    }

    void clearLine(){
        print("\033[K");
    }

    void setCursorPos(int line, int col){
        char posCmd[32];
        snprintf(posCmd, 32, "\033[%d;%dH", line, col);
        print(posCmd);
    }

	/** moves cursor position
	 * parameters may be negative
	 * @param lines number of lines to move
	 * @param cols number of cols to move
	 */
    void moveCursorPos(int lines, int spaces){
    	char posCmd[32];
    	if(lines==0 && spaces==0) return;
    	if(lines!=0){
    		if(lines>0){ // positive, so we move down
    			snprintf(posCmd, 32, "\033[%dB", lines);
    		}else if(lines<0){ // negative, so we move up
    			snprintf(posCmd, 32, "\033[%dA", (0-lines));
    		}
    	}
    	print(posCmd);
    	if(spaces!=0){
    		if(spaces>0){ // positive, so we move to the right
    			snprintf(posCmd, 32, "\033[%dC", spaces);
    		}else if(spaces<0){
    			snprintf(posCmd, 32, "\033[%dD", (0-spaces));
    		}
    	}
    	print(posCmd);
    }

    void saveCursorPos(){
        print("\033[s");
    }

    void restoreCursorPos(){
        print("\033[u");
    }

    private:
    char* readLineLastRead;
    protected:
};
#endif

#endif
