#include <Windows.h>
#include <tchar.h>
#include <CommCtrl.h>

#include "winmain.h"
#include "mainwindow.h"

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){
	Application myApp;
	myApp.hInstance=hInstance;
	MainWindow appWnd;
	appWnd.Register(hInstance);
	appWnd.Create(hInstance, _T("Window"));
	appWnd.Show();

	MSG msg;
	while(GetMessage(&msg, NULL, 0, 0)){
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}