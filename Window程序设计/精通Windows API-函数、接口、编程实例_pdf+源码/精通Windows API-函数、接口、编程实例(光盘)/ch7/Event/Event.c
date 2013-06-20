/* ************************************
*《精通Windows API》 
* 示例代码
* Event.c
* 7.2.1  演示使用Event同步线程
**************************************/

/* 头文件　*/
#include <windows.h>
#include <stdio.h>
/* 常量定义　*/
#define NUMTHREADS	3 
#define BUFFER_SIZE	16
#define FOR_TIMES	5
/* 全局变量　*/
HANDLE hEvent;					// 用于同步
BYTE lpSharedBuffer[16] = {0};	// 共享内存
/* 函数声明　*/
void UseEvents(void);
DWORD WINAPI EventFunction(LPVOID lpParam);

/*************************************
* int main(void)
* 功能	演示
*
* 参数	未使用
**************************************/
int main()
{
	UseEvents();
}

/*************************************
* void UseEvents(void) 
* 功能	演示Event的使用方法
*
* 参数	未使用
**************************************/
void UseEvents(void) 
{
	HANDLE hThread; 

	hEvent = CreateEvent( 
		NULL,			// 默认安全属性
		TRUE,			// 手工重置
		FALSE,			// 初始为未置位的
		NULL			// 未命名
		);
	// 判断是否创建成功
	if (hEvent == NULL) 
	{ 
		printf("CreateEvent failed (%d)\n", GetLastError());
		return;
	}
	// 创建线程
	hThread = CreateThread(NULL, 0, 
		EventFunction, 
		NULL,
		0, NULL); 
	if (hThread == NULL) 
	{
		printf("CreateThread failed (%d)\n", GetLastError());
		return;
	}	
	Sleep(2000); // 可以做一些其他处理
	// 向共享内存中复制数据
	CopyMemory(lpSharedBuffer,"Event",lstrlen("Event"));
	// 设置 Event 使ThreadFunction线程可以开始复制数据
	SetEvent(hEvent);
}

/*************************************
* DWORD WINAPI EventFunction(LPVOID lpParam)
* 功能	线程函数，读共享内存
*
* 参数	未使用
**************************************/
DWORD WINAPI EventFunction(LPVOID lpParam) 
{
	DWORD dwWaitResult;
	// 等待，直到事件被置位
	dwWaitResult = WaitForSingleObject( 
		hEvent,			// Event 句柄
		INFINITE);		// 无限等待
	if (dwWaitResult != WAIT_OBJECT_0) 
	{
		printf("Wait error: %d\n", GetLastError()); 
		return 0;
	}
	// 读共享内存
	printf(lpSharedBuffer);
	// 重置事件
	if (! ResetEvent(hEvent) ) 
	{ 
		printf("SetEvent failed (%d)\n", GetLastError());
		return 0;
	}
	return 1;
}