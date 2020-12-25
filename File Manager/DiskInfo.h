#include <Windows.h>

class DiskInfo
{
public:
	TCHAR path[256];
	TCHAR label[256];
	BOOL isAvailable;
	DWORDLONG allSpace, freeSpace, fullSpace;
	DiskInfo();
	void Update(TCHAR* _path);
	UINT GetNumberOfVolume(TCHAR drives[][4]);
};