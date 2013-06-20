/* ************************************
*《精通Windows API》
* 示例代码
* pro_s1.c
* 4.4.2  通过Mapping File在进程间共享内存
**************************************/
/* 头文件　*/
#include <windows.h>
#include <stdio.h>
#include <conio.h>
/* 预处理申明　*/
#define BUF_SIZE 256
/* 全局变量　*/
LPTSTR szName = TEXT("SharedFileMappingObject");
LPTSTR szMsg = TEXT("进程的消息");

/* ************************************
* int main(void)
* 功能	演示文件mapping共享内存，写入数据到共享内存
*
* 参数	无
*
* 返回值	0代表执行完成，代表发生错误
**************************************/
void main(int argc, PCHAR argv[])
{
	//文件映射句柄
	HANDLE hMapFile;
	//共享数据缓冲区指针
	LPTSTR pBuf;
	//创建命名的文件映射，不代表任务硬盘上的文件
	hMapFile = CreateFileMapping(
		INVALID_HANDLE_VALUE,
		NULL, 
		PAGE_READWRITE,
		0,
		BUF_SIZE, 
		szName);
	if (hMapFile == NULL || hMapFile == INVALID_HANDLE_VALUE) 
	{ 
		printf("CreateFileMapping error: %d\n",	GetLastError());
		return;
	}
	//创建View
	pBuf = (LPTSTR) MapViewOfFile(hMapFile,
		FILE_MAP_ALL_ACCESS,
		0,                   
		0,                   
		BUF_SIZE);
	if (pBuf == NULL) 
	{ 
		printf("MapViewOfFile error %d\n", 	GetLastError()); 
		return;
	}
	//将共享数据复制到文件映射中，如果运行时输入了参数则使用参数
	if(argc==1)
	{
		CopyMemory((PVOID)pBuf, szMsg, strlen(szMsg));
	}
	else
	{
		DWORD dwCopyLen = (lstrlen(argv[1])<BUF_SIZE) ? lstrlen(argv[1]): BUF_SIZE;
		CopyMemory((PVOID)pBuf, argv[1], dwCopyLen);
	}
	printf("运行程序，完成运行后，按任意键退出。");
	_getch();
	//取消映射，退出
	UnmapViewOfFile(pBuf);
	CloseHandle(hMapFile);
}