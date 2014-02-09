#ifndef WINMAIN_H
#define WINMAIN_H

#include <Windows.h>
#include <tchar.h>

class Application{
public:
protected:
	friend int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);

	HINSTANCE hInstance;
private:
};

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);

#endif