/* ************************************
*《精通Windows API》 
* 示例代码
* msg.c
* 6.5  动态链接库
**************************************/
/* 头文件　*/
#include <Windows.h>
#include <Psapi.h>
/* 链接　*/
#pragma comment (lib, "Psapi.lib") 
/* 函数声明　*/

// 使用__declspec(dllexport)声明导出函数
__declspec(dllexport) DWORD ExportExample(LPSTR szMsg, DWORD dwCode);

/*************************************
* DllMain
**************************************/
BOOL WINAPI DllMain(
					HINSTANCE hinstDLL,  // DLL模块的句柄
					DWORD fdwReason,     // 调用的情况
					LPVOID lpReserved )  // reserved
{
	// 在不同的情况下都会调用DllMain函数，分别处理
	switch( fdwReason ) 
	{ 
		// 加载Dll
	case DLL_PROCESS_ATTACH:
		{
			CHAR lpMainMoudleName[MAX_PATH];
			CHAR lpMessage[MAX_PATH+64];
			// 获取PID 和主模块名，将弹出消息框
			DWORD dwPID = GetCurrentProcessId();
			GetModuleBaseName(GetCurrentProcess(),NULL,lpMainMoudleName,MAX_PATH);
			wsprintf(lpMessage,"Process name: %s, PID: %u ",lpMainMoudleName,dwPID);
			MessageBox(NULL,lpMessage,"msg.dll",MB_OK);
			break;
		}
		// 新建线程
	case DLL_THREAD_ATTACH:
		break;
		// 线程退出
	case DLL_THREAD_DETACH:
		break;
		// 释放Dll
	case DLL_PROCESS_DETACH:

		break;
	}
	return TRUE;
}

/*************************************
* DWORD ExportExample(LPSTR szMsg, DWORD dwCode)
* 功能	导出函数，显示消息
*
* 参数	LPSTR szMsg	字符串； DWORD dwCode 整形
**************************************/
DWORD ExportExample(LPSTR szMsg, DWORD dwCode)
{
	LPVOID lpShowOut = HeapAlloc(GetProcessHeap(), 0, lstrlen(szMsg)+100);
	wsprintf(lpShowOut,"%s,%d",szMsg,dwCode);
	MessageBox(NULL,lpShowOut,"由导出函数弹出的消息！",MB_OK);
	HeapFree(GetProcessHeap(), 0, lpShowOut);
	return 0;
}