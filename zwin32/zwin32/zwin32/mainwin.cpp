#include <Windows.h>
#include <WindowsX.h>
#include <CommCtrl.h>
#include <cstdlib>
#include <cstring>
#include <string>
#include <tchar.h>
#include <list>

#include "../../../zcpu/zcpu/zcpu.h"
#include "../../../zmemory/zmemory/zmemory.h"


#include "mainwin.h"

using namespace std;

HINSTANCE MainWindow::hInstance;

list<string>& GUIInput::lineList = list<string>();
string& GUIInput::inputStream = string("");
bool GUIInput::returnHit = false;

char* MainWindow::gameText = NULL;
int MainWindow::gameTextLen = NULL;

long filesize(FILE *stream)
{
	long curpos, length;
	curpos = ftell(stream);
	fseek(stream, 0L, SEEK_END);
	length = ftell(stream);
	fseek(stream, curpos, SEEK_SET);
	return length;
}

namespace MWData{
	HBRUSH bgBrush;
	HBRUSH textFgBrush;
	HBRUSH textBgBrush;
	HFONT hTextFont;
};

using namespace MWData;

DWORD WINAPI ZCpuThreadProc(LPVOID lpParameter){
	FILE* storyFile = fopen("zork2.z3", "r");
	if (storyFile == NULL) return -1;
	zbyte* storyData = new zbyte[filesize(storyFile)];
	fread(storyData, filesize(storyFile), 1, storyFile);
	ZMemory zMem(storyData, filesize(storyFile));

	ZObjectTable zObj(&zMem);
	ZStack zStack;
	ZInOut zInOut;
	ZDictionary zDict(&zMem);
	ZCpu c(zMem, zStack, zObj, zInOut, zDict);
	c.startExecution();
	return 0;
}

LRESULT CALLBACK KbHookProc(int nCode, WPARAM wParam, LPARAM lParam){
	if (nCode < 0){
		return CallNextHookEx(NULL, nCode, wParam, lParam);
	}
	if ((char)wParam == '\r' || (char)wParam == '\n'){
		GUIInput::returnHit = true;
		return 0;
	}
	GUIInput::inputStream += ((char)wParam);
	char str[2] = { (char)wParam, 0 };
	if (str[0] != 0x7f)
		GUIInput::AppendToLastLine(string(str));
	else{
		string& lastString = GUIInput::lineList.back();
		lastString = lastString.substr(0, lastString.size() - 2);
	}
	return 0;
}

int MainWindow::WMCreate(HWND hWnd, WPARAM wParam, LPARAM lParam){
	bgBrush=CreateSolidBrush(RGB(0, 0, 0));
	SendMessage(hWnd, WM_PAINT, 0, 0);
	/*
	 * Create font
	 */
	hTextFont = CreateFont(
		14,
		7,
		0,
		0,
		FW_DONTCARE,
		FALSE,
		FALSE,
		FALSE,
		ANSI_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE,
		"Consolas"
		);
	/*
	Launch ZCpu thread
	*/
	CreateThread(NULL, NULL, ZCpuThreadProc, NULL, 0, NULL);
	SetTimer(hWnd, 1, 100, NULL);
	// create keyboard hook
	HHOOK hook = SetWindowsHookEx(WH_KEYBOARD, KbHookProc, MainWindow::hInstance, GetCurrentThreadId());
	return 0;
}

int MainWindow::WMPaint(HWND hWnd, WPARAM wParam, LPARAM lParam){
	PAINTSTRUCT ps;
	HDC hDC;
	hDC=BeginPaint(hWnd, &ps);
	RECT* lpWndRect=new RECT;
	/*
	 * Fill background
	 */
	if(!GetClientRect(hWnd, lpWndRect)){
		MessageBox(NULL, "Error : GetClientRect failed", NULL, NULL);
		return -1;
	}
	FillRect(hDC, lpWndRect, bgBrush);
	/*
	 * Draw text
	 */
	SelectFont(hDC, hTextFont);
	SetTextColor(hDC, RGB(255, 255, 255));
	SetBkColor(hDC, RGB(0, 0, 0));
	::DrawText(hDC, gameText, gameTextLen, lpWndRect, DT_WORDBREAK);
	delete lpWndRect;
	return 0;
}

int MainWindow::DrawText(HWND hWnd, WPARAM wParam, LPARAM lParam){
	int size = 0;
	for (list<string>::const_iterator it = GUIInput::lineList.begin(); it != GUIInput::lineList.end(); ++it){
		size += it->size()+2;
	}
	size += 1;
	if (gameText) delete[] gameText;
	string text("");
	int i = 0;
	for (list<string>::const_iterator it = GUIInput::lineList.begin(); it != GUIInput::lineList.end(); ++it){
		text += (*it);
		text += "\r\n";
	}
	gameText = new char[text.size() + 1];
	strcpy(gameText, text.c_str());
	gameText[text.size()] = NULL;
	gameTextLen = text.size();
	InvalidateRect(hWnd, NULL, false);
	return 0;
}

LRESULT CALLBACK MainWindow::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
	PAINTSTRUCT ps;
	HDC hDC;
	switch(uMsg){
	case WM_CREATE:
		MainWindow::WMCreate(hWnd, wParam, lParam);	
		break;
	case WM_TIMER:
		MainWindow::DrawText(hWnd, wParam, lParam);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_PAINT:
		MainWindow::WMPaint(hWnd, wParam, lParam);
		break;
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
		break;
	}
	return 0;
}