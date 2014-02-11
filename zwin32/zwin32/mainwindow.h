#ifndef MAINWINDOW_H
#define MAINWINDOW_H

namespace nsMainWindow{
	const TCHAR WndClass[]=_T("MainWnd");
}

class MainWindow{
public:
	MainWindow();
	virtual ~MainWindow();

	HWND Create(HINSTANCE hInstance,
				LPWSTR lpWndName,
				LPWSTR lpClsName=(LPWSTR)nsMainWindow::WndClass,
				HWND hwndParent=NULL,
				DWORD dwStyle = WS_OVERLAPPEDWINDOW,
				int x = CW_USEDEFAULT, int y = CW_USEDEFAULT,
				int w = 450, int h = 380);

	BOOL Show(int dwCmdShow = SW_SHOWNORMAL);

	ATOM Register(HINSTANCE hInstance);

	operator HWND();
protected:
	HWND m_hWnd;
	static HINSTANCE m_hInstance;
	LPWSTR m_ClassName;

	static int WM_Paint(HWND hWnd, WPARAM wParam, LPARAM lParam);
	static int WM_Create(HWND hWnd, WPARAM wParam, LPARAM lParam);

public:
	HINSTANCE GetInstance(){return m_hInstance;}

	static void ProcessOutput();
	static char GetKeyDown();
	static LRESULT CALLBACK MainWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
private:

	static BOOL CreateChildren(HWND hWnd);
	static BOOL MoveChildren(HWND hWnd);
};

#endif