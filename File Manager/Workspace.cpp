#include <Windows.h>
#include <CommCtrl.h>
#include <windowsx.h>
#include <Shlwapi.h>
#include <tchar.h>

#include "resource.h"
#include "Workspace.h"
#include "Formats.h"
#include "Conversion.h"

Workspace::Workspace() : hComboBox(nullptr), hListView(nullptr), hText(nullptr), isUsedNow(FALSE)
{
	memset(tCurrentPath, '\0', MAX_PATH_LENGHT);
}

// ComboBox Methods
BOOL Workspace::CreateComboBox(HWND hWnd, UINT uId)
{
	hComboBox = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		WC_COMBOBOX,
		TEXT("ComboBox"),
		WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST,
		0, 0, 0, 100,
		hWnd,
		(HMENU)uId,
		nullptr,
		nullptr);

	if (hComboBox == nullptr)
		return FALSE;
	else return TRUE;
}

void Workspace::ComboBoxInit()
{
	TCHAR drives[10][4];
	UINT uNumber = dDriveInfo.GetNumberOfVolume(drives);

	TCHAR A[40];
	memset(&A, 0, sizeof(A));

	for (size_t k = 0; k <= uNumber; k++)
	{
		wcscpy_s(A, sizeof(A) / sizeof(TCHAR), (TCHAR*)drives[k]);
		SendMessage(hComboBox, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)A);
	}

	SendMessage(hComboBox, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
	size_t length = ComboBox_GetTextLength(hComboBox);
	auto string = (LPWSTR)GlobalAlloc(GPTR, length + 1);
	ComboBox_GetText(hComboBox, string, length + 1);

	SetCurrentPath(string);
	dDriveInfo.Update(string);
}

// EditTex Methods
BOOL Workspace::CreateEditText(HWND hWnd, UINT uId)
{
	hText = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		WC_EDIT,
		nullptr,
		WS_CHILD | WS_VISIBLE | WS_BORDER | WS_DISABLED | WS_EX_LEFT,
		0, 0, 0, 0,
		hWnd,
		(HMENU)uId,
		nullptr,
		nullptr);

	if (hText == nullptr)
		return FALSE;
	else return TRUE;
}

// ListView Methods
BOOL Workspace::CreateListView(HWND hWnd, UINT uId)
{
	hListView = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		WC_LISTVIEW,
		TEXT("ListView"),
		WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_EDITLABELS | LVS_SHOWSELALWAYS,
		0, 0, 0, 0,
		hWnd,
		(HMENU)uId,
		nullptr,
		nullptr);

	ListView_SetExtendedListViewStyle(hListView, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	if (hListView == nullptr)
		return FALSE;
	else return TRUE;
}

void Workspace::ListViewInit()
{
	CreateImageList();

	LVCOLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

	lvc.fmt = LVCFMT_LEFT;
	lvc.cx = 290;
	lvc.pszText = TEXT("Name");
	lvc.iSubItem = 0;
	lvc.iOrder = 0;
	ListView_InsertColumn(hListView, 0, &lvc);

	lvc.cx = 100;
	lvc.pszText = TEXT("Size");
	lvc.iSubItem = 1;
	ListView_InsertColumn(hListView, 1, &lvc);

	lvc.cx = 100;
	lvc.pszText = TEXT("Date");
	lvc.iSubItem = 2;
	ListView_InsertColumn(hListView, 2, &lvc);

	UpdateList(dDriveInfo.path);
	SetCurrentPath(dDriveInfo.path);
}

void Workspace::InsertItem(UINT uItem, WIN32_FIND_DATA data, HANDLE hFile)
{
	LVITEM lvi;
	lvi.mask = LVIF_TEXT | LVIF_IMAGE;

	// Name column
	lvi.pszText = data.cFileName;
	lvi.iItem = uItem;
	lvi.iSubItem = 0;
	lvi.iImage = GetIconIndex(data);
	ListView_InsertItem(hListView, &lvi);

	// Size column
	if (lvi.iImage == 1 || lvi.iImage == 2)
		lvi.pszText = TEXT("<DIR>");
	else
	{
		LPWSTR s1 = GetLPWSTR(data.nFileSizeHigh * (MAXDWORD + 1) + data.nFileSizeLow), s2 = TEXT(" [B]");
		int lenght = lstrlen(s1) + lstrlen(s2);
		auto* tmp = new TCHAR[lenght];
		wcscpy(tmp, s1);
		wcscat(tmp, s2);

		lvi.pszText = tmp;
	}
	lvi.iSubItem = 1;
	ListView_SetItem(hListView, &lvi);

	// Date column
	lvi.pszText = GetLPWSTR(data.ftLastWriteTime);
	lvi.iSubItem = 2;
	ListView_SetItem(hListView, &lvi);
}

void Workspace::CreateImageList()
{
	HIMAGELIST hImageList = ImageList_Create(16, 16, ILC_COLOR32, 8, 0);
	HBITMAP hBmp = nullptr;
	// 0. File icon
	hBmp = LoadBitmap(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDB_FILE));
	ImageList_Add(hImageList, hBmp, (HBITMAP)nullptr);
	DeleteObject(hBmp);
	// 1. Folder icon
	hBmp = LoadBitmap(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDB_FOLDER));
	ImageList_Add(hImageList, hBmp, (HBITMAP)nullptr);
	DeleteObject(hBmp);
	// 2. Folder open icon
	hBmp = LoadBitmap(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDB_FOLDER_OPEN));
	ImageList_Add(hImageList, hBmp, (HBITMAP)nullptr);
	DeleteObject(hBmp);
	// 3. File exe icon
	hBmp = LoadBitmap(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDB_EXE));
	ImageList_Add(hImageList, hBmp, (HBITMAP)nullptr);
	DeleteObject(hBmp);
	// 4. File audio type icon
	hBmp = LoadBitmap(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDB_AUDIO));
	ImageList_Add(hImageList, hBmp, (HBITMAP)nullptr);
	DeleteObject(hBmp);
	// 5. File video type icon
	hBmp = LoadBitmap(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDB_VIDEO));
	ImageList_Add(hImageList, hBmp, (HBITMAP)nullptr);
	DeleteObject(hBmp);
	// 6. File Image type icon
	hBmp = LoadBitmap(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDB_IMAGE));
	ImageList_Add(hImageList, hBmp, (HBITMAP)nullptr);
	DeleteObject(hBmp);
	// 7. File document type icon
	hBmp = LoadBitmap(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDB_DOCUMENT));
	ImageList_Add(hImageList, hBmp, (HBITMAP)nullptr);
	DeleteObject(hBmp);

	ListView_SetImageList(hListView, hImageList, LVSIL_SMALL);
}

int Workspace::GetIconIndex(WIN32_FIND_DATA data)
{
	LPWSTR lpsPath = ConnectTwoString(this->tCurrentPath, (LPWSTR)data.cFileName, TRUE);
	LPWSTR lpsExt = PathFindExtension(lpsPath);

	if ((!_tcscmp(lpsExt, TEXT("")) || !_tcscmp(data.cFileName, TEXT(".."))) && data.nFileSizeLow == 0)
	{
		return !_tcscmp(data.cFileName, TEXT("..")) ? 2 : 1;
	}
	else
	{
		for (int i = 0; i < MAX_COUNT_FORMATS; i++)
		{
			if (!_tcscmp(lpsExt, tAudioFormat[i])) return 4;
			if (!_tcscmp(lpsExt, tVideoFormat[i])) return 5;
			if (!_tcscmp(lpsExt, tImageFormat[i])) return 6;
			if (!_tcscmp(lpsExt, tDocumentFormat[i])) return 7;
		}
	}
	return 0;
}

void Workspace::UpdateList(TCHAR* _path)
{
	auto* tOldPath = new TCHAR[256];
	memset(tOldPath, '\0', 256);
	wcscpy(tOldPath, tCurrentPath);

	ListView_DeleteAllItems(hListView);

	int length = lstrlen(_path) + lstrlen(TEXT("*.*"));
	auto* path = new TCHAR[length];
	wcscpy(path, _path);
	Edit_SetText(hText, path);
	SetCurrentPath(path);
	wcscat(path, TEXT("*.*"));


	WIN32_FIND_DATA data;
	HANDLE hFile = FindFirstFile(path, &data);

	SendMessage(hListView, LB_RESETCONTENT, 0, 0);

	int i = 0;
	while (FindNextFile(hFile, &data))
	{
		InsertItem(i, data, hFile);
		i++;
	}

	if (i == 0 && (MessageBox(nullptr, TEXT("Access denied!"), TEXT("Error"), MB_OK | MB_ICONERROR) == IDOK))
	{
		UpdateList(tOldPath);
	}

	FindClose(hFile);
}

void Workspace::SetCurrentPath(TCHAR* tNewPath)
{
	lstrcpyn(tCurrentPath, tNewPath, 256);
}

LPWSTR Workspace::GetCurrentPath()
{
	return tCurrentPath;
}

// Other Method
void Workspace::ResizeWorkspace(HWND hWnd, LPARAM lParam)
{
	SIZE sNewSize;
	sNewSize.cx = LOWORD(lParam) / 2;
	sNewSize.cy = HIWORD(lParam);
	if (isLeft)
	{
		MoveWindow(hComboBox, 0, 0, 50, 20, TRUE);
		MoveWindow(hText, 50, 0, sNewSize.cx - 50, 20, TRUE);
		MoveWindow(hListView, 0, 20, sNewSize.cx, sNewSize.cy - 25, TRUE);
	}
	else
	{
		MoveWindow(hComboBox, sNewSize.cx, 0, 50, 2, TRUE);
		MoveWindow(hText, sNewSize.cx + 50, 0, sNewSize.cx - 50, 20, TRUE);
		MoveWindow(hListView, sNewSize.cx, 20, sNewSize.cx, sNewSize.cy - 25, TRUE);
	}
}

BOOL Workspace::Initialization(HWND hWnd, UINT uComboId, UINT uListId, UINT uTextId, BOOL isLeft)
{
	this->isLeft = isLeft;

	InitCommonControls();

	if (!CreateComboBox(hWnd, uComboId))
	{
		MessageBox(nullptr, TEXT("Function CreateComboBox failure!"), TEXT("Error"), MB_OK | MB_ICONERROR);
		return FALSE;
	}
	ComboBoxInit();

	if (!CreateEditText(hWnd, uTextId))
	{
		MessageBox(nullptr, TEXT("Function CreateEditText failure!"), TEXT("Error"), MB_OK | MB_ICONERROR);
		return FALSE;
	}

	if (!CreateListView(hWnd, uListId))
	{
		MessageBox(nullptr, TEXT("Function CreateComboBox failure!"), TEXT("Error"), MB_OK | MB_ICONERROR);
		return FALSE;
	}

	ListViewInit();

	auto hNormalFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	SendMessage(hComboBox, WM_SETFONT, (WPARAM)hNormalFont, 0);
	SendMessage(hText, WM_SETFONT, (WPARAM)hNormalFont, 0);
	SendMessage(hListView, WM_SETFONT, (WPARAM)hNormalFont, 0);

	return TRUE;
}
