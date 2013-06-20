/* ************************************
*《精通Windows API》
* 示例代码
* handle_path.cpp
* 4.4.3	通过文件句柄获取文件路径
**************************************/
/* 头文件*/
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <string.h>
#include <psapi.h>
/* 预处理申明*/
#pragma comment (lib, "Psapi.lib")
#define BUFSIZE 512
/* 函数申明*/
BOOL GetFileNameFromHandle(HANDLE hFile) ;

/* ************************************
* BOOL GetFileNameFromHandle(HANDLE hFile) 
* 功能	从文件句柄获取文件路径
*
* 参数	ANDLE hFile，需要获得路径的文件句柄
*
* 返回值BOOL 是否成功。
**************************************/
BOOL GetFileNameFromHandle(HANDLE hFile) 
{
	TCHAR pszFilename[MAX_PATH+1];
	HANDLE hFileMap;
	PVOID pMem;

	// 获得文件大小,并决断是否为
	DWORD dwFileSizeHigh = 0;
	DWORD dwFileSizeLow = GetFileSize(hFile, &dwFileSizeHigh); 
	if( dwFileSizeLow == 0 && dwFileSizeHigh == 0 )
	{
		printf("不能map文件大小为的文件.\n");
		return FALSE;
	}
	// 创建mapping对象
	hFileMap = CreateFileMapping(hFile, 
		NULL, 
		PAGE_READONLY,
		0, 
		1,
		NULL);
	if (!hFileMap) 
	{
		printf("CreateFileMapping error: %d",GetLastError());
		return FALSE;
	}

	pMem = MapViewOfFile(hFileMap, FILE_MAP_READ, 0, 0, 1);
	if (!pMem) 
	{
		printf("MapViewOfFile error: %d",GetLastError());
		return FALSE;
	}
	// 从mapping对象获得文件名
	if (0 == GetMappedFileName (GetCurrentProcess(), 
		pMem, 
		pszFilename,
		MAX_PATH))
	{
		printf("GetMappedFileName error: %d",GetLastError());
		return FALSE;
	}
	// 将设备名转换为路径
	TCHAR szTemp[BUFSIZE] = {0};
	if (0 == GetLogicalDriveStrings(BUFSIZE-1, szTemp)) 
	{
		printf("GetLogicalDriveStrings error: %d",GetLastError());
		return FALSE;
	}
	TCHAR szName[MAX_PATH];
	TCHAR szDrive[3] = {0};
	BOOL bFound = FALSE;
	PTCHAR p = szTemp;
	do 
	{
		CopyMemory(szDrive,p,2*sizeof(TCHAR));
		// 通过路径查找设备名
		if (!QueryDosDevice(szDrive, szName, BUFSIZE))
		{
			printf("QueryDosDevice error: %d",GetLastError());
			return FALSE;
		}
		UINT uNameLen = lstrlen(szName);
		if (uNameLen < MAX_PATH) 
		{
			//比较驱动器的设备名与文件设备名是否匹配
			bFound = strncmp(pszFilename, szName,uNameLen) == 0;
			if (bFound) 
			{
				//如果匹配，说明已经找到，构造路径。
				TCHAR szTempFile[MAX_PATH];
				wsprintf(szTempFile,
					TEXT("%s%s"),
					szDrive,
					pszFilename+uNameLen);
				lstrcpy(pszFilename, szTempFile);
			}
		}
		// 循环到下一个NULL
		while (*p++);
	} while (!bFound && *p);

	UnmapViewOfFile(pMem);
	CloseHandle(hFileMap);
	printf("File path is %s\n", pszFilename);
	return TRUE;
}

/* ************************************
* int main()
* 功能	查找第一个目录中第一个txt文件
*		打开文件，并根据文件句柄获得文件路径。
*
* 参数	未使用
*
* 返回值0表示成功，表示失败。
**************************************/
int main()
{
	HANDLE hFile;
	HANDLE hFind;
	WIN32_FIND_DATA wfd;
	hFind = FindFirstFile("*.txt",&wfd);
	if(hFind == INVALID_HANDLE_VALUE)
	{
		printf("can not find a file");
		return 1;
	}
	//CloseHandle(hFind);
	printf("find %s at current dir\n",wfd.cFileName);
	hFile = CreateFile(wfd.cFileName, 
		GENERIC_READ | GENERIC_WRITE,
		0, 
		NULL,
		OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		printf("create file error, %d",GetLastError());
	}
	else
	{
		GetFileNameFromHandle(hFile) ;
	}
	CloseHandle(hFile);
	return 0;
}
