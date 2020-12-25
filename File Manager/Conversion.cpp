#include "WinMain.h"

// CHAR* convert to WCHAR*
LPWSTR GetLPWSTR(LPSTR in)
{
	const size_t cSize = strlen(in) + 1;
	auto* out = new wchar_t[cSize];
	mbstowcs(out, in, cSize);

	return out;
}

// WCHAR* convert to CHAR*
LPSTR GetLPSTR(LPWSTR in)
{
	const size_t cSize = wcslen(in) + 1;
	auto* out = new char[cSize];
	wcstombs(out, in, cSize);
	return out;
}

// DWORD convert to LPWCSTR
LPWSTR GetLPWSTR(DWORD in)
{
	wchar_t out[256] = { 0 };
	memset(out, '\0', 256);

	_ultow(in, out, 10);

	return out;
}

// FILETIME convert to LPWSTR;
LPWSTR GetLPWSTR(const FILETIME ft)
{
	SYSTEMTIME sTime;
	FileTimeToSystemTime(&ft, &sTime);				// Conversion from file time to system time

	auto* year = new TCHAR[4];
	wcscpy(year, GetLPWSTR(sTime.wYear));
	auto* month = new TCHAR[2];
	wcscpy(month, GetLPWSTR(sTime.wMonth));
	auto* day = new TCHAR[2];
	wcscpy(day, GetLPWSTR(sTime.wDay));
	auto* hour = new TCHAR[2];
	wcscpy(hour, GetLPWSTR(sTime.wHour));
	auto* minute = new TCHAR[2];
	wcscpy(minute, GetLPWSTR(sTime.wMinute));

	int length = lstrlen(day) + lstrlen(month) + lstrlen(year) + lstrlen(hour)
		+ lstrlen(minute) + 2 * lstrlen(TEXT("-")) + lstrlen(TEXT(" ")) + lstrlen(TEXT(":"));

	auto* out = new TCHAR[length];
	wcscpy(out, day);
	wcscat(out, TEXT("-"));
	wcscat(out, month);
	wcscat(out, TEXT("-"));
	wcscat(out, year);
	wcscat(out, TEXT(" "));
	wcscat(out, hour);
	wcscat(out, TEXT(":"));
	wcscat(out, minute);

	return out;
}

#define MAX_PATH_LENGHT 256

// Connect two TCHAR*
LPWSTR ConnectTwoString(TCHAR* tString1, TCHAR* tString2, bool tString2IsFile)
{
	TCHAR* tNewPath = new TCHAR[MAX_PATH_LENGHT];
	memset(tNewPath, '\0', MAX_PATH_LENGHT);

	if (lstrlen(tString1) + lstrlen(tString2) <= MAX_PATH_LENGHT)
	{
		wcscpy(tNewPath, tString1);
		wcscat(tNewPath, tString2);
		if (!tString2IsFile)
		{
			wcscat(tNewPath, TEXT("\\"));
		}
		return tNewPath;
	}

	return nullptr;
}

LPWSTR GetPastPath(TCHAR* tString1)
{
	TCHAR* tNewPath = new TCHAR[MAX_PATH_LENGHT];
	memset(tNewPath, '\0', MAX_PATH_LENGHT);

	int position = lstrlen(tString1) - 2;
	for (int i = position; i >= 2; i--)
	{
		if (tString1[i] == '\\')
		{
			tString1[i + 1] = '\0';
			break;
		}
	}
	wcscpy(tNewPath, tString1);

	return tNewPath;
}