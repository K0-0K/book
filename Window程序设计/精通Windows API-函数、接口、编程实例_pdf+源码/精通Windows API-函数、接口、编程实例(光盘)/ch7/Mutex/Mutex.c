/* ************************************
*《精通Windows API》 
* 示例代码
* Mutex.c
* 7.2.2  演示使用Mutex同步线程
**************************************/

/* 头文件　*/
#include <windows.h>
#include <stdio.h>
/* 常量定义　*/
#define NUM_THREADS	4 
/* 全局变量　*/
DWORD dwCounter = 0;
HANDLE hMutex; 
/* 函数声明　*/
void UseMutex(void);
DWORD WINAPI MutexThread(LPVOID lpParam);

/*************************************
* int main(void)
* 功能	演示
*
* 参数	未使用
**************************************/
int main()
{
	UseMutex();
}
/*************************************
* void UseMutex(void)
* 功能	演示 Mutex 的使用方法
*
* 参数	未使用
**************************************/
void UseMutex(void) 
{
	INT i;
	HANDLE hThread;

#ifdef MUTEX
	// 创建 Mutex
	hMutex = CreateMutex( 
		NULL,                       // 默认安全属性
		FALSE,                      // 初始化为未被拥有
		NULL);                      // 未命名
	if (hMutex == NULL) 
	{
		printf("CreateMutex error: %d\n", GetLastError());
	}
#endif
	// 创建多个线程
	for(i = 0; i < NUM_THREADS; i++) 
	{		
		hThread = CreateThread(NULL, 0, 
			MutexThread, 
			NULL, 
			0, NULL); 
		if (hThread == NULL) 
		{
			printf("CreateThread failed (%d)\n", GetLastError());
			return;
		}
	}
	Sleep(1000);
}

/*************************************
* DWORD WINAPI MutexThread(LPVOID lpParam) 
* 功能	线程函数，读共享内存
*
* 参数	未使用
**************************************/
DWORD WINAPI MutexThread(LPVOID lpParam) 
{
	
#ifdef MUTEX 
	DWORD dwWaitResult;
	dwWaitResult = WaitForSingleObject( 
		hMutex,			// 句柄
		INFINITE);		// 无限等待

	switch (dwWaitResult) 
	{
	case WAIT_OBJECT_0: 
#endif
		// 等待随机长的时间，各个线程等待的时间将不一致
		Sleep(rand()%100);
		// 得到互斥对象后 访问共享数据
		printf("counter: %d\n",dwCounter);
		// 互斥对象保证同一时间只有一个线程在访问dwCounter
		dwCounter++;

#ifdef MUTEX
		// 释放 Mutex
		if(!ReleaseMutex(hMutex))
		{
			printf("Release Mutex error: %d\n", GetLastError()); 
		}
		break; 
	default: 
		printf("Wait error: %d\n", GetLastError()); 
		ExitThread(0); 
	}
#endif
	return 1;
}
