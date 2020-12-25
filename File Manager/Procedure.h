#include <Windows.h>

LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

LRESULT CommandProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void CopySelectedItemToCopyBuffer(HWND hList, TCHAR * tPath);

void CopyOrMoveFiles(HWND hList, TCHAR *tPath);

LRESULT NotifyProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

BOOL CALLBACK NewFileDialogProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void CreateNewFileOrFolder(TCHAR * tPath);

BOOL CALLBACK OpenFolderDialogProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);