/* ************************************
*《精通Windows API》 
* 示例代码
* child.c
* 6.2.1  创建进程、获取进程相关信息、获取启动参数
**************************************/
/* 头文件　*/
#include <windows.h>
#include <stdio.h>
/* 宏定义　*/
#define MyAlloc(size) HeapAlloc(GetProcessHeap(),0,size)
#define MyFree(lpMem) HeapFree(GetProcessHeap(),0,lpMem)
/* 结构体定义　*/
typedef struct _PROCESS_INFO
{
	DWORD dwPid;
	HANDLE hProcess;
	DWORD dwPrioClass;
	DWORD dwHandleCount;
	DWORD dwAffinityMask;
	SIZE_T dwWorkingSetSizeMax;
	SIZE_T dwWorkingSetSizeMin;
	LPWSTR szwCommandLine;
	STARTUPINFO sti;
}PROCESS_INFO, *LPPROCESS_INFO;
/* 全局变量　*/
HANDLE hMySelf;
/* 函数声明　*/
DWORD GetProcessInfo(LPPROCESS_INFO lppi);

/*************************************
* int WinMain(
	HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow
* 功能	演示获取进程信息，在进程中获取命令行参数等
*
**************************************/
int WinMain(
	HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow
)
{
	PROCESS_INFO pi;
	INT argc;
	WCHAR **argv;
	DWORD i;
	DWORD dwBufferSize = lstrlen(lpCmdLine)+MAX_PATH+1024;
	LPSTR szShowBuffer = (LPSTR)MyAlloc(dwBufferSize);

	hMySelf = hInstance;
	// 显示直接从WinMain函数参数得到的信息
	wsprintf(szShowBuffer,
		"启动参数\n实例句柄：%.8X，命令行参数：%s，显示选项：%.8X",
		hInstance,lpCmdLine,nCmdShow);
	MessageBox(NULL,szShowBuffer,"WinMain函数参数",MB_OK);
	// 使用自定义的函数获取相关信息
	GetProcessInfo(&pi);
	// 将命令行参数分离
	argv = CommandLineToArgvW(pi.szwCommandLine,&argc);
	// 字符处理，并显示
	*szShowBuffer = '\x00';	
	for(i=0; i<argc; i++)
	{
		DWORD dwBufferSize = lstrlenW(*argv)+1;
		LPSTR szMBArgv = MyAlloc(dwBufferSize);
		WideCharToMultiByte(CP_ACP,0,*argv,-1,szMBArgv,dwBufferSize,NULL,NULL);
		argv++;
		lstrcat(szShowBuffer,"\n");
		lstrcat(szShowBuffer,szMBArgv);
		MyFree(szMBArgv);
	}
	MessageBox(NULL,szShowBuffer,"参数",MB_OK);
	MyFree(szShowBuffer);
	// 打印其他信息 TODO
	return 0;
}
/*************************************
* DWORD GetProcessInfo(LPPROCESS_INFO lppi)
* 功能	获取进程相关信息，保存在PROCESS_INFO结构中
*
* 参数	LPPROCESS_INFO lppi	用于保存相关信息
**************************************/
DWORD GetProcessInfo(LPPROCESS_INFO lppi)
{
	// PID
	lppi->dwPid = GetCurrentProcessId();
	// 句柄
	lppi->hProcess = GetCurrentProcess();
	// 优先级
	lppi->dwPrioClass = GetPriorityClass(hMySelf);
	// 句柄记数
	//windows xp sp1 sdk 新增的API GetProcessHandleCount
	//lppi->dwHandleCount 
	//	= GetProcessHandleCount(lppi->hProcess,&lppi->dwHandleCount);
	
	// AffinityMask
	GetProcessAffinityMask(hMySelf,
		&lppi->dwAffinityMask,
		NULL);	
	// WorkingSetSize
	GetProcessWorkingSetSize(hMySelf,
		&lppi->dwWorkingSetSizeMin,
		&lppi->dwWorkingSetSizeMax);	
	lppi->szwCommandLine =  GetCommandLineW();
	// 启动信息
	GetStartupInfo(&lppi->sti);
	return 0;
}
