#include "DiskInfo.h"

DiskInfo::DiskInfo()
{
	memset(this->path, '\0', 256);
	memset(this->label, '\0', 256);
	this->isAvailable = FALSE;
	this->allSpace = 0;
	this->freeSpace = 0;
	this->fullSpace = 0;

	this->Update(TEXT("C:\\"));
}

void DiskInfo::Update(TCHAR* _path)
{
	lstrcpyn(this->path, _path, 256);
	memset(this->label, '\0', 256);

	isAvailable = TRUE;

	GetDiskFreeSpaceEx(
		this->path,
		nullptr,
		(ULARGE_INTEGER*)&allSpace,
		(ULARGE_INTEGER*)&freeSpace);

	fullSpace = allSpace - freeSpace;
}

UINT DiskInfo::GetNumberOfVolume(TCHAR drives[][4])
{
	memset(drives, '\0', sizeof(TCHAR) * 10 * 4);

	DWORD mydrives = 100;
	TCHAR lpBuffer[100];

	memset(lpBuffer, '\0', 100);

	DWORD test = GetLogicalDriveStrings(mydrives, lpBuffer);

	if (test)
	{
		int k = 0;
		for (size_t i = 0; i < mydrives; i += 4)
		{
			if (isalpha(lpBuffer[i]))
			{
				drives[k][0] = lpBuffer[i];
				drives[k][1] = TEXT(':');
				drives[k][2] = TEXT('\\');
				drives[k][3] = TEXT('\0');

				k++;
			}

			if ((lpBuffer[i] == '\0') && (lpBuffer[i + 1] == '\0') && (lpBuffer[i + 2] == '\0') && (lpBuffer[i + 3] == '\0'))
			{
				return k;
			}
		}
	}
	return 0;
}
