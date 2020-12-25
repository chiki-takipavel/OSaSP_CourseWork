#include <Windows.h>

class CustomWindow
{
protected:
	HWND hWnd;
	SIZE sWindowSize;
public:
	CustomWindow() : hWnd(NULL) {}
	BOOL Initialization(HINSTANCE hInstance, SIZE sSize);
	WPARAM MsgCommunicationLoop(HACCEL hAccelerators);
};

