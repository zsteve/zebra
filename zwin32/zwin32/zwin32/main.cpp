#include <Windows.h>
#include <cstdlib>
#include <cstring>
#include <string>
#include <tchar.h>

#include "resource.h"

#include "mainwin.h"

int WINAPI WinMain(HINSTANCE hInstance,
					HINSTANCE hPrevInstance,
					LPSTR lpCmdLine,
					int nCmdShow){
	static TCHAR szWindowClass[]=_T("Window");
	MainWindow mainWin;
	MainWindow::hInstance=(hInstance);
	WNDCLASSEX wcex;
	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= MainWindow::WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_INFORMATION));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_INFORMATION));

	if(!RegisterClassEx(&wcex)){
		MessageBox(NULL, _T("Error : RegisterClassEx() Failed"), _T("Error"), NULL);
		return 1;
	}
	static TCHAR szTitle[]=_T("ZWin32");
	HMENU hMainMenu = LoadMenu(hInstance, MAKEINTRESOURCE(IDR_MENU1));
	HWND hWnd=CreateWindow(
		szWindowClass,
		szTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		500, 400, 
		NULL,
		(HMENU)hMainMenu,
		hInstance,
		NULL);
	if(!hWnd){
		MessageBox(NULL, _T("Error : CreateWindow() Failed"), _T("Error"), NULL);
		return -1;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	MSG msg;
	while(GetMessage(&msg, NULL, 0, 0)){
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}