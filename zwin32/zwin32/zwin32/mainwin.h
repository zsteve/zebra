#ifndef WINMAIN_H
#define WINMAIN_H

#include "../../../zinout/zinout/zinout.h"
#include <Windows.h>
#include <list>

using namespace std;

class GUIInput{
public:
	friend class MainWindow;
	friend LRESULT CALLBACK KbHookProc(int nCode, WPARAM wParam, LPARAM lParam);
	static void Print(const char* str){
		int i = 0;
		string thisString("");
		while (str[i]){
			if (IsLF(str[i])){
				// line feed detected : we must skip a line
				AddLine(thisString);
				thisString = "";
				i++;
				continue;
			}
			thisString += str[i];
			i++;
		}
		AppendToLastLine(thisString);
	}

	static list<string>& lineList;
	static string& inputStream;
	static bool returnHit;
private:
	static const int maxLines = 200;

	static void AddLine(const char* str){
		lineList.push_back(string(str));
		if (lineList.size() > maxLines){
			lineList.pop_front();
		}
	}

	static void AddLine(string str){
		AddLine(str.c_str());
	}

	static void AppendToLastLine(const char* str){
		string& last = lineList.back();
		last.append(str);
	}

	static void AppendToLastLine(string str){
		AppendToLastLine(str.c_str());
	}
	
	static bool IsLF(char c){
		if (c != '\n' && c != '\r') return false;
		return true;
	}
protected:
};

class MainWindow{
public:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static int WMCreate(HWND hWnd, WPARAM wParam, LPARAM lParam);
	static int WMPaint(HWND hWnd, WPARAM wParam, LPARAM lParam);
	static int DrawText(HWND hWnd, WPARAM wParam, LPARAM lParam);
	static HINSTANCE hInstance;
	friend class ZInOut;
	friend class GUIInput;
	friend LRESULT CALLBACK KbHookProc(int nCode, WPARAM wParam, LPARAM lParam);
private:
	static char* gameText;
	static int gameTextLen;
};
#endif