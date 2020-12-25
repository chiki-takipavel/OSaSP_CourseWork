#include "CustomWindow.h"
#include "WinMain.h"
#include "Procedure.h"

BOOL CustomWindow::Initialization(HINSTANCE hInstance, SIZE size)
{
	WNDCLASSEX  wndClass;
	LPCWSTR className = TEXT("WindowClass");

	ZeroMemory(&wndClass, sizeof(wndClass));

	wndClass.cbSize = sizeof(wndClass);
	wndClass.lpfnWndProc = (WNDPROC)::WindowProcedure;
	wndClass.lpszClassName = className;
	wndClass.hInstance = hInstance;	
	wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wndClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_HDD));
	wndClass.lpszMenuName = MAKEINTRESOURCE(IDR_CUSTOM_MENU);
	wndClass.style = CS_DBLCLKS;
	
	if (RegisterClassEx(&wndClass) == NULL) return FALSE;

	hWnd = CreateWindow(
		className,
		TEXT("File Manager"),
		WS_OVERLAPPEDWINDOW,
		100, 100,
		size.cx, size.cy,
		NULL,
		NULL,
		hInstance,
		NULL);

	if (hWnd == NULL) return FALSE;

	ShowWindow(hWnd, SW_SHOWNORMAL);
	UpdateWindow(hWnd);	

	return TRUE;
}

WPARAM CustomWindow::MsgCommunicationLoop(HACCEL hAccelerators)
{
	MSG msg;
	while (GetMessage(&msg, hWnd, 0, 0) > 0)
	{
		TranslateAccelerator(hWnd, hAccelerators, &msg);
		TranslateMessage(&msg);	
		DispatchMessage(&msg);
	}
	return msg.wParam;
}