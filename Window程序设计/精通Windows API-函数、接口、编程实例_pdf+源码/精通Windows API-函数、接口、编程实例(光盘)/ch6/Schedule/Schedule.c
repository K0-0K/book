/* ************************************
*《精通Windows API》
* 示例代码
* schedule.c
* 6.3.2 挂起、恢复、切换、终止线程
**************************************/
/* 头文件*/
#include <windows.h>
#include <stdio.h>

/*************************************
* DWORD WINAPI ThreadProc( LPVOID lpParam )
* 功能	线程函数
*		间隔循环打印输出
*
* 参数LPVOID lpParam 指向DWORD
**************************************/
DWORD WINAPI ThreadProc( LPVOID lpParam )
{
	LPDWORD pData;
	DWORD i = 0;
	// 参数数据类型
	pData = (LPDWORD)lpParam;
	// 循环打印输出
	for(i; i<10; i++)
	{
		Sleep(100);		// 每毫秒打印一次
		printf("TID = %u,\t Parameters = %u\t i = %u\n",
			GetCurrentThreadId(), *pData, i);
	}
	ExitThread(i);
	return 0;
}

/*************************************
* void main()
* 功能主线程函数，演示线程调度
**************************************/
void main()
{
	DWORD dwData;
	DWORD dwThreadId[2];
	HANDLE hThread[2];
	// 创建线程
	dwData = 1;
	hThread[0] = CreateThread(
		NULL,0,
		ThreadProc,
		&dwData,
		CREATE_SUSPENDED,	// 挂起新建的进程
		&dwThreadId[0]);
	if (hThread[0] == NULL)
	{
		ExitProcess(0);
	}

	/// 创建线程
	dwData = 2;
	hThread[1] = CreateThread(
		NULL,0,
		ThreadProc,
		&dwData,
		0,					// 默认标志
		&dwThreadId[1]);
	if (hThread[1] == NULL)
	{
		ExitProcess(1);
	}

	// 等待200毫秒，恢复线程的执行
	Sleep(200);
	ResumeThread(hThread[0]);
	// 挂起线程的执行
	SuspendThread(hThread[1]);
	// 等待300毫秒，终止线程，恢复线程
	Sleep(300);
	TerminateThread(hThread[0],0);
	ResumeThread(hThread[1]);

	//等待所有线程执行结束
	WaitForMultipleObjects(2, hThread, TRUE, INFINITE);
	// 关闭所有线程的句柄
	CloseHandle(hThread[0]);
	CloseHandle(hThread[1]);
}