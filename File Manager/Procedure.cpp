#include <Windows.h>
#include <windowsx.h>
#include <CommCtrl.h>
#include <wchar.h>
#include <string.h>
#include <Shlwapi.h>
#include <tchar.h>

#include "resource.h"
#include "Procedure.h"
#include "Workspace.h"
#include "Conversion.h"

Workspace wLeftWorkspace, wRightWorkspace;
HBITMAP hBmp;
TCHAR* tBuffer0 = new TCHAR[256], * tBuffer1 = new TCHAR[256], ** tCopyBuffer = NULL;
int countCopyBuffer = 0;
BOOL cutMode;
POINT pPoint;

LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
		if (!wLeftWorkspace.Initialization(hWnd, ID_LEFTCOMBOBOX, ID_LEFTLISTVIEW, ID_LEFTTEXT, TRUE))
		{
			MessageBox(nullptr, TEXT("Function wLeftWorkspace.Initialization failure!"), TEXT("Error"), MB_OK | MB_ICONERROR);
		}
		wLeftWorkspace.isUsedNow = TRUE;
		if (!wRightWorkspace.Initialization(hWnd, ID_RIGHTCOMBOBOX, ID_RIGHTLISTVIEW, ID_RIGHTTEXT, FALSE))
		{
			MessageBox(nullptr, TEXT("Function wRightWorkspace.Initialization failure!"), TEXT("Error"), MB_OK | MB_ICONERROR);
		}
		break;
	case WM_COMMAND:
		CommandProcedure(hWnd, uMsg, wParam, lParam);
		break;
	case WM_NOTIFY:
		NotifyProcedure(hWnd, uMsg, wParam, lParam);
		break;
	case WM_SIZE:
		wLeftWorkspace.ResizeWorkspace(hWnd, lParam);
		wRightWorkspace.ResizeWorkspace(hWnd, lParam);
		break;
	case WM_CLOSE:
		if (MessageBox(nullptr, TEXT("Do you want close the window?"), TEXT("Question"), MB_OKCANCEL | MB_ICONQUESTION) == IDOK)
		{
			DestroyWindow(hWnd);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}

LRESULT CommandProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
	case ID_LEFTCOMBOBOX:
		wLeftWorkspace.isUsedNow = TRUE;
		wRightWorkspace.isUsedNow = FALSE;

		if (HIWORD(wParam) == CBN_SELENDOK)
		{
			ComboBox_GetCurSel(wLeftWorkspace.hComboBox);
			size_t length = ComboBox_GetTextLength(wLeftWorkspace.hComboBox);

			auto lpsString = (LPWSTR)GlobalAlloc(GPTR, length + 1);
			ComboBox_GetText(wLeftWorkspace.hComboBox, lpsString, length + 1);

			wLeftWorkspace.dDriveInfo.Update(lpsString);
			wLeftWorkspace.UpdateList(wLeftWorkspace.dDriveInfo.path);

			GlobalFree(lpsString);
		}
		break;
	case ID_RIGHTCOMBOBOX:
		wRightWorkspace.isUsedNow = TRUE;
		wLeftWorkspace.isUsedNow = FALSE;

		if (HIWORD(wParam) == CBN_SELENDOK)
		{
			ComboBox_GetCurSel(wRightWorkspace.hComboBox);
			size_t length = ComboBox_GetTextLength(wRightWorkspace.hComboBox);

			auto lpsString = (LPWSTR)GlobalAlloc(GPTR, length + 1);
			ComboBox_GetText(wRightWorkspace.hComboBox, lpsString, length + 1);

			wRightWorkspace.dDriveInfo.Update(lpsString);
			wRightWorkspace.UpdateList(wRightWorkspace.dDriveInfo.path);

			GlobalFree(lpsString);
		}
		break;
	case ID_FILE_NEW:
		DialogBox(NULL, MAKEINTRESOURCE(IDD_FILE_NEW_DIALOG), hWnd, (DLGPROC)NewFileDialogProcedure);
		break;
	case ID_FILE_OPEN:
		DialogBox(NULL, MAKEINTRESOURCE(IDD_FILE_NEW_DIALOG), hWnd, (DLGPROC)OpenFolderDialogProcedure);
		break;
	case ID_FILE_EXIT:
		SendMessage(hWnd, WM_CLOSE, NULL, NULL);
		break;
	case ID_COPY_MODE:
	case ID_EDIT_COPY:
		cutMode = FALSE;
		if (wLeftWorkspace.isUsedNow) 
			CopySelectedItemToCopyBuffer(wLeftWorkspace.hListView, wLeftWorkspace.GetCurrentPath());
		else 
			CopySelectedItemToCopyBuffer(wRightWorkspace.hListView, wRightWorkspace.GetCurrentPath());
		break;
	case ID_CUT_MODE:
	case ID_EDIT_CUT:
		cutMode = TRUE;
		if (wLeftWorkspace.isUsedNow) 
			CopySelectedItemToCopyBuffer(wLeftWorkspace.hListView, wLeftWorkspace.GetCurrentPath());
		else 
			CopySelectedItemToCopyBuffer(wRightWorkspace.hListView, wRightWorkspace.GetCurrentPath());
		break;
	case ID_PASTE_MODE:
	case ID_EDIT_PASTE:
		if (wLeftWorkspace.isUsedNow)
		{
			CopyOrMoveFiles(wLeftWorkspace.hListView, wLeftWorkspace.GetCurrentPath());
		}
		else
		{
			CopyOrMoveFiles(wRightWorkspace.hListView, wRightWorkspace.GetCurrentPath());
		}
		wLeftWorkspace.UpdateList(wLeftWorkspace.GetCurrentPath());
		wRightWorkspace.UpdateList(wRightWorkspace.GetCurrentPath());
		break;
	case ID_DEL_MODE:
	case ID_EDIT_DELETE:
		SendMessage(hWnd, WM_COMMAND, ID_EDIT_COPY, NULL);
		if (tCopyBuffer)
		{
			for (int i = 0; i < countCopyBuffer; i++)
			{
				if (PathIsDirectory(tCopyBuffer[i]))
				{
					if (!PathIsDirectoryEmpty(tCopyBuffer[i]))
					{
						MessageBox(nullptr, TEXT("Directory is not empty! Remove content!"), TEXT("Warning"), MB_OK | MB_ICONWARNING);
					}
					RemoveDirectory(tCopyBuffer[i]);
				}
				else DeleteFile(tCopyBuffer[i]);

				wLeftWorkspace.UpdateList(wLeftWorkspace.GetCurrentPath());
				wRightWorkspace.UpdateList(wRightWorkspace.GetCurrentPath());

				delete[] tCopyBuffer[i];
			}
			delete[] tCopyBuffer;
		}
		break;
	case ID_INFO_MODE:
	case ID_HELP_ABOUTFILEMANAGER:
		MessageBox(nullptr, TEXT("Copyright (c) 2020 by Pavel Miskevich"), TEXT("About File Manager"), MB_OK | MB_ICONINFORMATION);
		break;
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}

void CopySelectedItemToCopyBuffer(HWND hList, TCHAR* tPath)
{
	countCopyBuffer = ListView_GetSelectedCount(hList);

	tCopyBuffer = new TCHAR * [countCopyBuffer];
	auto position = ListView_GetNextItem(hList, -1, LVNI_SELECTED);
	for (int i = 0; i < countCopyBuffer; i++)
	{
		tCopyBuffer[i] = new TCHAR[256];

		memset(tBuffer0, '\0', 256);
		ListView_GetItemText(hList, position, 0, tBuffer0, 256);
		tCopyBuffer[i] = ConnectTwoString(tPath, tBuffer0, TRUE);

		position = ListView_GetNextItem(hList, position, LVNI_SELECTED);
	}
}

void CopyOrMoveFiles(HWND hList, TCHAR* tPath)
{
	if (tCopyBuffer)
	{
		for (int i = 0; i < countCopyBuffer; i++)
		{
			memset(tBuffer0, '\0', 256);
			tBuffer0 = PathFindFileName(tCopyBuffer[i]);

			memset(tBuffer1, '\0', 256);
			tBuffer1 = ConnectTwoString(tPath, tBuffer0, TRUE);

			if (cutMode) 
				MoveFile(tCopyBuffer[i], tBuffer1);
			else 
				CopyFile(tCopyBuffer[i], tBuffer1, FALSE);

			delete[] tCopyBuffer[i];
		}
		delete[] tCopyBuffer;
		tCopyBuffer = nullptr;
		countCopyBuffer = 0;
	}
}

LRESULT NotifyProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	auto lpnmItem = (LPNMITEMACTIVATE)lParam;
	bool isBack = false;

	if ((((LPNMHDR)lParam)->hwndFrom) == GetDlgItem(hWnd, ID_LEFTLISTVIEW))
	{
		switch (((LPNMHDR)lParam)->code)
		{
		case NM_RCLICK:
			wLeftWorkspace.isUsedNow = TRUE;
			wRightWorkspace.isUsedNow = FALSE;

			ListView_GetItemPosition(wLeftWorkspace.hListView, lpnmItem->iItem, &pPoint);
			ClientToScreen(wLeftWorkspace.hListView, &pPoint);

			TrackPopupMenu(
				GetSubMenu(LoadMenu(NULL, MAKEINTRESOURCE(IDR_CUSTOM_MENU)), 1),
				0,
				pPoint.x + 50, pPoint.y,
				0,
				hWnd,
				NULL);

			break;
		case NM_CLICK:
			wLeftWorkspace.isUsedNow = TRUE;
			wRightWorkspace.isUsedNow = FALSE;
			break;

		case NM_DBLCLK:
			memset(tBuffer0, '\0', 256);
			memset(tBuffer1, '\0', 256);
			isBack = false;

			ListView_GetItemText(wLeftWorkspace.hListView, lpnmItem->iItem, 0, tBuffer0, 256);
			if (!_tcscmp(tBuffer0, TEXT("..")))
				isBack = true;
			
			if (isBack)
			{
				tBuffer1 = GetPastPath(wLeftWorkspace.GetCurrentPath());
			}
			else
			{
				tBuffer1 = ConnectTwoString(wLeftWorkspace.GetCurrentPath(), tBuffer0, FALSE);
			}

			memset(tBuffer0, '\0', 256);
			ListView_GetItemText(wLeftWorkspace.hListView, lpnmItem->iItem, 1, tBuffer0, 256);

			if (!_tcscmp(tBuffer0, TEXT("<DIR>")))
			{
				wLeftWorkspace.UpdateList(tBuffer1);
			}
			else
			{
				ShellExecute(nullptr, TEXT("open"), tBuffer1, nullptr, nullptr, SW_SHOWNORMAL);
			}
			break;
		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
	}
	else if ((((LPNMHDR)lParam)->hwndFrom) == GetDlgItem(hWnd, ID_RIGHTLISTVIEW))
	{
		switch (((LPNMHDR)lParam)->code)
		{
		case NM_RCLICK:
			wRightWorkspace.isUsedNow = TRUE;
			wLeftWorkspace.isUsedNow = FALSE;

			ListView_GetItemPosition(wRightWorkspace.hListView, lpnmItem->iItem, &pPoint);
			ClientToScreen(wRightWorkspace.hListView, &pPoint);

			TrackPopupMenu(
				GetSubMenu(LoadMenu(NULL, MAKEINTRESOURCE(IDR_CUSTOM_MENU)), 1),
				0,
				pPoint.x + 50, pPoint.y,
				0,
				hWnd,
				NULL);

			break;
		case NM_CLICK:
			wRightWorkspace.isUsedNow = TRUE;
			wLeftWorkspace.isUsedNow = FALSE;
			break;

		case NM_DBLCLK:
			memset(tBuffer0, '\0', 256);
			memset(tBuffer1, '\0', 256);
			isBack = false;

			ListView_GetItemText(wRightWorkspace.hListView, lpnmItem->iItem, 0, tBuffer0, 256);
			if (!_tcscmp(tBuffer0, TEXT("..")))
				isBack = true;

			if (isBack)
			{
				tBuffer1 = GetPastPath(wRightWorkspace.GetCurrentPath());
			}
			else
			{
				tBuffer1 = ConnectTwoString(wRightWorkspace.GetCurrentPath(), tBuffer0, FALSE);
			}

			memset(tBuffer0, '\0', 256);
			ListView_GetItemText(wRightWorkspace.hListView, lpnmItem->iItem, 1, tBuffer0, 256);

			if (!_tcscmp(tBuffer0, TEXT("<DIR>")))
			{
				wRightWorkspace.UpdateList(tBuffer1);
			}
			else
			{
				ShellExecute(nullptr, TEXT("open"), tBuffer1, nullptr, nullptr, SW_SHOWNORMAL);
			}
			break;
		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
	}
}

BOOL CALLBACK NewFileDialogProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HWND hTextField = GetDlgItem(hWnd, IDC_EDIT1);

	switch (uMsg)
	{
	case WM_INITDIALOG:
		if (wLeftWorkspace.isUsedNow == TRUE) 
			SetWindowText(hWnd, wLeftWorkspace.GetCurrentPath());
		else 
			SetWindowText(hWnd, wRightWorkspace.GetCurrentPath());
		break;
	case WM_COMMAND:
		memset(tBuffer0, '\0', 256);
		memset(tBuffer1, '\0', 256);

		switch (LOWORD(wParam))
		{
		case IDOK:
			Edit_GetText(hTextField, tBuffer0, 256);

			if (wLeftWorkspace.isUsedNow)
			{
				CreateNewFileOrFolder(wLeftWorkspace.GetCurrentPath());
			}
			else
			{
				CreateNewFileOrFolder(wRightWorkspace.GetCurrentPath());
			}
			wLeftWorkspace.UpdateList(wLeftWorkspace.GetCurrentPath());
			wRightWorkspace.UpdateList(wRightWorkspace.GetCurrentPath());
		case IDCANCEL:
			EndDialog(hWnd, NULL);
			break;
		default:
			break;
		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

void CreateNewFileOrFolder(TCHAR* tPath)
{
	tBuffer1 = ConnectTwoString(tPath, tBuffer0, TRUE);
	memset(tBuffer0, '\0', 256);
	tBuffer0 = PathFindExtension(tBuffer1);

	if (!_tcscmp(tBuffer0, TEXT("")))
	{
		wcscat(tBuffer1, TEXT("\\"));
		CreateDirectory(tBuffer1, nullptr);
	}
	else
	{
		HANDLE hAppend = CreateFile(tBuffer1,
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			nullptr,
			CREATE_NEW,
			FILE_ATTRIBUTE_NORMAL,
			nullptr);
		CloseHandle(hAppend);
	}
}

BOOL CALLBACK OpenFolderDialogProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HWND hTextField = GetDlgItem(hWnd, IDC_EDIT1);

	switch (uMsg)
	{
	case WM_INITDIALOG:
		SetWindowText(hWnd, TEXT("Open"));
		break;
	case WM_COMMAND:
		memset(tBuffer0, '\0', 10);
		memset(tBuffer1, '\0', 256);

		switch (LOWORD(wParam))
		{
		case IDOK:
			Edit_GetText(hTextField, tBuffer0, 256);

			tBuffer1 = PathFindExtension(tBuffer0);

			if (!_tcscmp(tBuffer1, TEXT("")))
			{
				if (wLeftWorkspace.isUsedNow)
					wLeftWorkspace.UpdateList(ConnectTwoString(tBuffer0, TEXT("\\"), TRUE));
				else
					wRightWorkspace.UpdateList(ConnectTwoString(tBuffer0, TEXT("\\"), TRUE));
			}
			else
				ShellExecute(nullptr, TEXT("open"), tBuffer0, nullptr, nullptr, SW_SHOWNORMAL);

		case IDCANCEL:
			EndDialog(hWnd, NULL);
			break;
		default:
			break;
		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}