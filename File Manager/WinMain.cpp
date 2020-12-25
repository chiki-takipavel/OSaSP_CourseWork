#include "CustomWindow.h"
#include "resource.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	SIZE sSize;
	sSize.cx = 1000; sSize.cy = 600;

	CustomWindow cWindow;	
	if (!cWindow.Initialization(hInstance, sSize))
	{
		MessageBox(nullptr, TEXT("Function cWindow.Initialization failure!"), TEXT("Error"), MB_OK | MB_ICONERROR);
	}

	HACCEL hAccelerators = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));

	if (!hAccelerators)
	{
		MessageBox(nullptr, TEXT("Cannot load accelerators!"), TEXT("Error"), MB_OK | MB_ICONERROR);
	}

	return (int)cWindow.MsgCommunicationLoop(hAccelerators);
}