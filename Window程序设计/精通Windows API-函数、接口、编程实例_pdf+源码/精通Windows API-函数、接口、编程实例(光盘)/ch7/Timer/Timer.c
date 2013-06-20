/* ************************************
*《精通Windows API》 
* 示例代码
* Timer.c
* 6.5.5  演示使用可等待计时器
**************************************/
/* 预编译定义 *Windows 2000*　*/
#define _WIN32_WINNT 0x0500
/* 头文件　*/
#include <windows.h>
#include <stdio.h>
/* 常量定义　*/
#define ONE_SECOND 10000000			// 秒
/* 类型定义　*/
// TimerAPCProc 参数
typedef struct _APC_PROC_ARG {
	TCHAR *szText;
	DWORD dwValue;
} APC_PROC_ARG;

/*************************************
* VOID CALLBACK TimerAPCProc(
*				LPVOID lpArg,
*				DWORD dwTimerLowValue,
*				DWORD dwTimerHighValue )
* 功能	演示定时器的使用
*
* 参数	未使用
**************************************/
VOID CALLBACK TimerAPCProc(
						   LPVOID lpArg,
						   DWORD dwTimerLowValue,
						   DWORD dwTimerHighValue )

{
	APC_PROC_ARG *pApcData = (APC_PROC_ARG *)lpArg;
	// 显示信息
	printf( "Message: %s\nValue: %d\n\n", pApcData->szText,
		pApcData->dwValue);
	MessageBeep(MB_OK);
}

/*************************************
* void main( void ) 
* 功能	演示定时器的使用
*
* 参数	未使用
**************************************/
void main( void ) 
{
	HANDLE          hTimer;
	BOOL            bSuccess;
	INT64			qwDueTime;
	LARGE_INTEGER   liDueTime;
	APC_PROC_ARG    ApcData;

	ApcData.szText = "Message to apc proc.";
	ApcData.dwValue = 1;
	hTimer = CreateWaitableTimer(
		NULL,                   // 默认安全属性
		FALSE,                  // 自动设置
		"MyTimer" );			// 命名
	if ( !hTimer ) 
	{
		printf("CreateWaitableTimer failed with Error %d.", 
			GetLastError() );
		return;
	} 
	else 
	{
		__try 
		{
			// 五秒
			qwDueTime = -5 * ONE_SECOND;
			// 转换为 LARGE_INTEGER 数据结构
			liDueTime.LowPart  = (DWORD) ( qwDueTime & 0xFFFFFFFF );
			liDueTime.HighPart = (LONG)  ( qwDueTime >> 32 );

			bSuccess = SetWaitableTimer(
				hTimer,           // 计时器句柄
				&liDueTime,       // 延迟时间，第一次置位的时间
				2000,             // 计时器间隔，每2秒置位一次
				TimerAPCProc,     // 置位时触发调用的例程
				&ApcData,         // 例程参数
				FALSE );          // 不重置挂起系统
			// 判断是否创建成功
			if ( bSuccess ) 
			{
				for ( ; ApcData.dwValue <= 4; ApcData.dwValue ++ ) 
				{
					// 第二个参数当设置为 TRUE，表示当计时器到达，APC被调用时返回
					SleepEx(
						INFINITE,     // 无限等待
						TRUE );
					// 每返回一次，dwValue值递增。
				}
			} 
			else 
			{
				printf( "SetWaitableTimer failed with Error %d.",
					GetLastError() );
			}
		} 
		__finally 
		{
			CloseHandle( hTimer );
		}
	}
}