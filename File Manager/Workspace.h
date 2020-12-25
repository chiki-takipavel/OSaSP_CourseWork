#pragma comment(lib, "comctl32.lib")
#include <Windows.h>
#include "DiskInfo.h"

#define MAX_PATH_LENGHT 256

class Workspace
{
protected:
	TCHAR tCurrentPath[MAX_PATH_LENGHT];
	BOOL isLeft;
private:
	BOOL CreateComboBox(HWND hWnd, UINT uId);
	void ComboBoxInit();
	BOOL CreateEditText(HWND hWnd, UINT uId);
	BOOL CreateListView(HWND hWnd, UINT uId);
	void ListViewInit();
	void InsertItem(UINT uItem, WIN32_FIND_DATA data, HANDLE hFile);
	void CreateImageList();
	int GetIconIndex(WIN32_FIND_DATA data);
public:
	HWND hComboBox, hListView, hText;
	DiskInfo dDriveInfo;
	BOOL isUsedNow;
	Workspace();
	BOOL Initialization(HWND hWnd, UINT uComboId, UINT uListId, UINT uTextId, BOOL isLeft);
	void UpdateList(TCHAR *path);
	void SetCurrentPath(TCHAR *tNewPath);
	LPWSTR GetCurrentPath();
	void ResizeWorkspace(HWND hWnd, LPARAM lParam);
};