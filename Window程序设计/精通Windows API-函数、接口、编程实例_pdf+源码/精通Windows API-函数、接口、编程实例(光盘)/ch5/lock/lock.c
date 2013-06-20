/* ************************************
*《精通Windows API》 
* 示例代码
* lock.c
* 5.2.4 堆管理
**************************************/

/* 头文件　*/
#include <windows.h>
#include <stdio.h>
/* 全局变量　*/
HANDLE hHeap;
/* 函数申明　*/
DWORD WINAPI ThreadProc(LPVOID lpParameter);

/*************************************
* int main(int argc, PCHAR argv[])
* 功能	演示堆的使用
*
* 参数	argv[1]如果为“-s”那么使用进程堆 
*		argv[1]如果为“-a”那么创建一个可变大小的堆
*		argv[1]如果为其他，那么创建有最大大小的堆。
*
* 2007年10月
*
**************************************/
int main(int argc, PCHAR argv[])
{	
	DWORD dwThreadId;
	HANDLE hThread;
	DWORD i;
	LPVOID lpMem;
	hHeap = HeapCreate(HEAP_NO_SERIALIZE,0,0);
	if(hHeap != NULL)
	{
		printf("线程1：创建堆成功\n");
	}
	else
	{
		printf("线程1：创建堆失败(%d)\n",GetLastError());
		return 1;
	}
	hThread = CreateThread( 
		NULL, 0,
		ThreadProc,
		NULL, 0,
		&dwThreadId); 
	if(hThread == NULL)
	{
		printf("线程1：创建线程失败(%d)\n",GetLastError());
		return 1;
	}
	printf("线程1：创建线程成功\n");
	for(i = 0; i<30; i++)
	{
		Sleep(100);
		lpMem = HeapAlloc(hHeap,HEAP_ZERO_MEMORY,40960);
		if(lpMem == NULL)
		{
			printf("线程1：分配内存错误(%d)\n",GetLastError());
			continue;
		}
		if(!HeapFree(hHeap,HEAP_NO_SERIALIZE,lpMem))
		{
			printf("线程1：释放内存错误(%d)\n",GetLastError());
			continue;
		}
		printf("线程1：可以从堆分配到内存\n");		
	}
	return 0;
}

DWORD WINAPI ThreadProc(
						LPVOID lpParameter
						)
{
	LPVOID lpMem;
	printf("线程2：等待1秒\n");
	Sleep(1000);	
	if(HeapLock(hHeap))
	{
		printf("线程2：堆已经被锁定\n");
		lpMem = HeapAlloc(hHeap,HEAP_ZERO_MEMORY,40960);
		if(lpMem == NULL)
		{
			goto error;
		}
		printf("线程2：已经成功分配了内存\n");
		printf("线程2：等待1秒\n");
		Sleep(1000);
		if(HeapFree(hHeap,HEAP_NO_SERIALIZE,lpMem))
		{
			printf("线程2：已经成功释放了内存\n");
		}
		else
		{
			goto error;
		}
		if(HeapUnlock(hHeap))
		{
			printf("线程2：已经解除了对堆的锁定\n");
		}
		else
		{
			goto error;
		}
	}
	else
	{
		goto error;
	}
	return 0;
error:
	printf("线程2：错误(%d)，退出\n",GetLastError());
	ExitProcess(0);
}