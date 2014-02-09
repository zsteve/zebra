#include <Windows.h>
#include <tchar.h>
#include <CommCtrl.h>

#include <cstdlib>

#include "mainwindow.h"
#include "positioner.h"
#include "defines.h"

#include "io.h"

#include "../../zcpu/zcpu/zcpu.h"
#include "../../zglobal/zglobaldefines.h"

HINSTANCE MainWindow::m_hInstance;

namespace nsMainWindow{
	HWND hClickMeBtn;
	HFONT hMainFont=NULL;

	int winW=450, winH=380;

	Positioner pos;
}

namespace nsGameData{
	LPWSTR lpGameText=NULL;

	IO* ioObj=IO::getInstance();
}

/* GLOBAL ZVERSION */
int zVersion = 3;

namespace nsGameCode{

	using namespace nsGameData;

	HANDLE hGameThread;

	long filesize(FILE *stream)
	{
	   long curpos, length;
	   curpos = ftell(stream);
	   fseek(stream, 0L, SEEK_END);
	   length = ftell(stream);
	   fseek(stream, curpos, SEEK_SET);
	   return length;
	}

	DWORD WINAPI GameThreadProc(LPVOID lpParam){
		FILE* storyFile=fopen("zork2.z3", "r");
		if(storyFile==NULL) return -1;
		zbyte* storyData=new zbyte[filesize(storyFile)];
		fread(storyData, filesize(storyFile), 1, storyFile);
		ZMemory zMem(storyData, filesize(storyFile));
		ZObjectTable zObj(&zMem);
		ZStack zStack;
		ZInOut zInOut;
		ZDictionary zDict(&zMem);
		ZCpu c(zMem, zStack, zObj, zInOut, zDict);
		c.startExecution();
	}

	string gameTextString("");

	string& CompileString(){
		gameTextString.clear();
		for(list<string>::const_iterator it=ioObj->getOutBuffer().begin();
			it!=ioObj->getOutBuffer().end();
			++it){
			gameTextString+=*it;
			gameTextString+='\n';
		}
		return gameTextString;
	}
}

using namespace nsGameData;
using namespace nsGameCode;
using namespace nsMainWindow;

MainWindow::MainWindow() : m_hWnd(0){
	
}

MainWindow::~MainWindow(){

}

HWND MainWindow::Create(HINSTANCE hInstance,
			LPWSTR lpWndName,
			LPWSTR lpClsName,
			HWND hwndParent,
			DWORD dwStyle,
			int x, int y,
			int w, int h){
	m_hWnd=CreateWindowEx(NULL,
						lpClsName,
						lpWndName,
						dwStyle,
						x, y,
						w, h,
						hwndParent,
						NULL,
						hInstance,
						NULL);
	if(!m_hWnd) MessageBox(NULL, NULL, NULL, NULL);
	m_hInstance=hInstance;
	m_ClassName=lpClsName;
	return m_hWnd;
}

BOOL MainWindow::Show(int dwCmdShow){
	BOOL CanShow = ::ShowWindow(m_hWnd, dwCmdShow);
	if(CanShow) return TRUE;
	return FALSE;
}

ATOM MainWindow::Register(HINSTANCE hInstance){
	WNDCLASSEX wcx;
	wcx.cbSize			= sizeof(WNDCLASSEX);
	wcx.style			= CS_HREDRAW | CS_VREDRAW;
	wcx.lpfnWndProc		= MainWindowProc;
	wcx.cbClsExtra		= NULL;
	wcx.cbWndExtra		= NULL;
	wcx.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
	wcx.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wcx.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);
	wcx.lpszMenuName	= NULL;
	wcx.lpszClassName	= nsMainWindow::WndClass;
	wcx.hInstance		= hInstance;
	wcx.hIconSm			= LoadIcon(NULL, IDI_APPLICATION);

	return RegisterClassEx(&wcx);
}

MainWindow::operator HWND(){
	return m_hWnd;
}

BOOL MainWindow::CreateChildren(HWND hWnd){
	return TRUE;
}

BOOL MainWindow::MoveChildren(HWND hWnd){
	SetWindowPos(hClickMeBtn, HWND_TOP, pos.GetAbsPosX(0.5)-BUTTON_W/2, pos.GetAbsPosY(0.5), BUTTON_W, BUTTON_H, SWP_SHOWWINDOW);
	return TRUE;
}

int MainWindow::WM_Create(HWND hWnd, WPARAM wParam, LPARAM lParam){
	HDC hDC;
	// set up Positioner object
	{
		RECT wndRect;
		GetWindowRect(hWnd, &wndRect);
		winW=wndRect.right-wndRect.left;
		winH=wndRect.bottom-wndRect.top;
		pos.SetW(winW);
		pos.SetH(winH);
	}
	// set font
	hMainFont=CreateFont(14, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE,\
				ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,\
				ANTIALIASED_QUALITY, FF_DONTCARE, _T("Lucida Console"));
	hDC=GetDC(hWnd);
	SelectObject(hDC, (HGDIOBJ)hMainFont);
	SetBkColor(hDC, RGB(0, 0, 0));
	ReleaseDC(hWnd, hDC);
	SendMessage(hWnd, WM_SETFONT, (WPARAM)hMainFont, TRUE);
	CreateChildren(hWnd);

	// set up game
	nsGameCode::hGameThread=CreateThread(NULL, NULL, nsGameCode::GameThreadProc, NULL, NULL, NULL);

	SetTimer(hWnd, 1, 20, NULL);
	return 0;
}

int MainWindow::WM_Paint(HWND hWnd, WPARAM wParam, LPARAM lParam){
	PAINTSTRUCT ps;
	HDC hDC;
	RECT wndRect;

	int ww, wh;

	hDC=BeginPaint(hWnd, &ps);
	GetClientRect(hWnd, &wndRect);

	ww=wndRect.right-wndRect.left;
	wh=wndRect.bottom-wndRect.top;

	// create memory DC
	HDC hMemDC=CreateCompatibleDC(hDC);
	SetTextColor(hMemDC, RGB(255, 255, 255));
	SetBkColor(hMemDC, RGB(0, 0, 0));
	SelectObject(hMemDC, (HGDIOBJ)hMainFont);
	HBITMAP hMemBitmap=CreateCompatibleBitmap(hMemDC, ww, wh);
	HBITMAP hMemOldBitmap=(HBITMAP)SelectObject(hMemDC, (HGDIOBJ)hMemBitmap);
	FillRect(hMemDC, &wndRect, (HBRUSH)GetStockObject(BLACK_BRUSH));

	int height=DrawText(hMemDC, lpGameText, -1, &wndRect, DT_TOP | DT_WORDBREAK | DT_EDITCONTROL);
	if(height>wh){
		FillRect(hMemDC, &wndRect, (HBRUSH)GetStockObject(BLACK_BRUSH));
		int i=wndRect.top;
		wndRect.top=wh-height;
		DrawText(hMemDC, lpGameText, -1, &wndRect, DT_TOP | DT_WORDBREAK | DT_EDITCONTROL);
		wndRect.top=i;
	}

	BitBlt(hDC, 0, 0, ww, wh, hMemDC, 0, 0, SRCCOPY);
	SelectObject(hMemDC, (HGDIOBJ)hMemOldBitmap);
	DeleteObject((HGDIOBJ)hMemBitmap);
	ReleaseDC(hWnd, hMemDC);
	EndPaint(hWnd, &ps);
	return 0;
}

void MainWindow::ProcessOutput(){
	string& gameText=CompileString();
	if(lpGameText){
		delete[] lpGameText;
	}
	lpGameText=new TCHAR[gameText.size()+1];
	MultiByteToWideChar(CP_ACP,
						MB_COMPOSITE,
						gameText.c_str(),
						-1,
						lpGameText,
						gameText.size());
}

char MainWindow::GetKeyDown(){
	unsigned char kbState[256];
	static unsigned char lastKey=0;
	unsigned char curKey=0;
	unsigned short cbuf[2];
	short state=0;
	static HKL kbLayout=NULL;

	for(int vkCode=0; vkCode<256; vkCode++){
		state=GetAsyncKeyState(vkCode);
		kbState[vkCode]=state;
		if(!curKey){
			if((state & 0x8000)){
				curKey=vkCode;
			}
		}
	}
	if(curKey || !lastKey){
		lastKey = curKey;
	}
	if(!kbLayout){
		kbLayout = GetKeyboardLayout(NULL);
	}

	if(ToAsciiEx(curKey, MapVirtualKeyEx(curKey, 0, kbLayout), kbState, cbuf, 0, kbLayout)){
		return (char)(cbuf[0]&255);
	}else{
		return 0;
	}
}

LRESULT CALLBACK MainWindow::MainWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
	switch(uMsg){
	HDC hDC;
	case WM_DESTROY:
		PostQuitMessage(WM_QUIT);
		break;
	case WM_CREATE:
		WM_Create(hWnd, wParam, lParam);
		break;
	case WM_CHAR:
		{
			ioObj->put_in(GetKeyDown());
		}
		break;
	case WM_KEYDOWN:
		switch(LOWORD(wParam)){
		case VK_RETURN:
			if(ioObj->has_input()){
				ioObj->end_input();
			}
			break;
		}
		break;
	case WM_SIZE:
		winH=HIWORD(lParam);
		winW=LOWORD(lParam);
		pos.SetH(winH);
		pos.SetW(winW);
		MoveChildren(hWnd);
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam)){

		}
		break;
	case WM_TIMER:
		switch(LOWORD(wParam)){
		case 1:
			ProcessOutput();
			RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE);
			break;
		}
	case WM_PAINT:
		WM_Paint(hWnd, wParam, lParam);
		break;
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}