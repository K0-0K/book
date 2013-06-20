/* ************************************
*《精通Windows API》 
* 示例代码
* ctrl.c
* 6.6	服务
* 服务控制程序，启动、停止服务，向服务发送请求
**************************************/
/* 头文件　*/
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
/* 全局变量　*/
extern SC_HANDLE schService;	// 在init.c中定义，下同
extern SC_HANDLE schSCManager;
extern LPTSTR szServiceName;

/*************************************
* BOOL StartSampleService(SC_HANDLE schSCManager,LPTSTR szServiceName) 
* 功能	启动服务
*
* 参数	SC_HANDLE schSCManager	SCM 句柄
*		LPTSTR szServiceName	服务名
**************************************/
BOOL StartSampleService(SC_HANDLE schSCManager,LPTSTR szServiceName) 
{ 
	SC_HANDLE schService;
	SERVICE_STATUS_PROCESS ssStatus; 
	DWORD dwOldCheckPoint; 
	DWORD dwStartTickCount;
	DWORD dwWaitTime;
	DWORD dwBytesNeeded;
	// 打开服务
	schService = OpenService( 
		schSCManager,			// SCM database 
		szServiceName,          // service name
		SERVICE_ALL_ACCESS); 
	if (schService == NULL) 
	{ 
		return 0; 
	}
	// 启动服务
	if (!StartService(
		schService,  // handle to service 
		0,           // number of arguments 
		NULL) )      // no arguments 
	{
		printf("Service start error (%u).\n",GetLastError()); 
		return 0; 
	}
	else 
	{
		printf("Service start pending.\n"); 
	}

	// 验证状态
	if (!QueryServiceStatusEx( 
		schService,             // handle to service 
		SC_STATUS_PROCESS_INFO, // info level
		(LPBYTE)&ssStatus,              // address of structure
		sizeof(SERVICE_STATUS_PROCESS), // size of structure
		&dwBytesNeeded ) )              // if buffer too small
	{
		return 0; 
	}

	// tick count & checkpoint.
	dwStartTickCount = GetTickCount();
	dwOldCheckPoint = ssStatus.dwCheckPoint;
	// 查询状态，确定 PENDING 状态结束
	while (ssStatus.dwCurrentState == SERVICE_START_PENDING) 
	{ 
		// 等待一段时间
		dwWaitTime = ssStatus.dwWaitHint / 10;
		if( dwWaitTime < 1000 )
			dwWaitTime = 1000;
		else if ( dwWaitTime > 10000 )
			dwWaitTime = 10000;
		Sleep( dwWaitTime );
		// 再次查询
		if (!QueryServiceStatusEx( 
			schService,             // handle to service 
			SC_STATUS_PROCESS_INFO, // info level
			(LPBYTE)&ssStatus,              // address of structure
			sizeof(SERVICE_STATUS_PROCESS), // size of structure
			&dwBytesNeeded ) )              // if buffer too small
			break; 
		if ( ssStatus.dwCheckPoint > dwOldCheckPoint )
		{
			// 进程创建中
			dwStartTickCount = GetTickCount();
			dwOldCheckPoint = ssStatus.dwCheckPoint;
		}
		else
		{
			if(GetTickCount()-dwStartTickCount > ssStatus.dwWaitHint)
			{
				// WaitHint 时间到
				break;
			}
		}
	} 
	// 关闭句柄
	CloseServiceHandle(schService); 
	// 判断是否创建成功（状态由PENDING变为RUNNING）
	if (ssStatus.dwCurrentState == SERVICE_RUNNING) 
	{
		printf("StartService SUCCESS.\n"); 
		return 1;
	}
	else 
	{ 
		printf("\nService not started. \n");
		printf("  Current State: %d\n", ssStatus.dwCurrentState); 
		printf("  Exit Code: %d\n", ssStatus.dwWin32ExitCode); 
		printf("  Service Specific Exit Code: %d\n", 
			ssStatus.dwServiceSpecificExitCode); 
		printf("  Check Point: %d\n", ssStatus.dwCheckPoint); 
		printf("  Wait Hint: %d\n", ssStatus.dwWaitHint); 
		return 0;
	} 
}

/*************************************
* DWORD StopService( SC_HANDLE hSCM, 
LPTSTR szServiceName, 
BOOL fStopDependencies, 
DWORD dwTimeout )  
* 功能	停止服务
*
* 参数	SC_HANDLE hSCM			SCM 句柄
*		LPTSTR szServiceName	服务名
*		BOOL fStopDependencies	是否结束依赖的服务
*		DWORD dwTimeout			超时
**************************************/
DWORD StopService(SC_HANDLE hSCM, 
				  LPTSTR szServiceName, 
				  BOOL fStopDependencies, 
				  DWORD dwTimeout ) 
{
	SERVICE_STATUS_PROCESS ssp;
	SERVICE_STATUS ss;
	DWORD dwStartTime = GetTickCount();
	DWORD dwBytesNeeded;
	// 打开服务
	SC_HANDLE hService = OpenService( 
		hSCM,          // SCM 句柄 
		szServiceName,          // 服务名
		SERVICE_ALL_ACCESS); 

	// 查询状态，确定是否已经停止
	if ( !QueryServiceStatusEx( 
		hService, 
		SC_STATUS_PROCESS_INFO,
		(LPBYTE)&ssp, 
		sizeof(SERVICE_STATUS_PROCESS),
		&dwBytesNeeded ) )
	{
		return GetLastError();
	}
	if ( ssp.dwCurrentState == SERVICE_STOPPED ) 
	{
		return ERROR_SUCCESS;
	}
	// 如果是 STOP_PENDING 状态，则只需等待
	while ( ssp.dwCurrentState == SERVICE_STOP_PENDING ) 
	{
		Sleep( ssp.dwWaitHint );
		// 循环查询，直到状态改变
		if ( !QueryServiceStatusEx( 
			hService, 
			SC_STATUS_PROCESS_INFO,
			(LPBYTE)&ssp, 
			sizeof(SERVICE_STATUS_PROCESS),
			&dwBytesNeeded ) )
		{
			return GetLastError();
		}
		if ( ssp.dwCurrentState == SERVICE_STOPPED )
		{
			return ERROR_SUCCESS;
		}
		if ( GetTickCount() - dwStartTime > dwTimeout )
		{
			return ERROR_TIMEOUT;
		}
	}

	// 先结束依赖服务
	if ( fStopDependencies ) 
	{
		DWORD i;
		DWORD dwBytesNeeded;
		DWORD dwCount;

		LPENUM_SERVICE_STATUS   lpDependencies = NULL;
		ENUM_SERVICE_STATUS     ess;
		SC_HANDLE               hDepService;

		// 使用 0 大小的 buf,获取buf的大小
		// 如果 EnumDependentServices 直接返回成功，说明没有依赖服务
		if ( !EnumDependentServices( hService, SERVICE_ACTIVE, 
			lpDependencies, 0, &dwBytesNeeded, &dwCount ) ) 
		{
			if ( GetLastError() != ERROR_MORE_DATA )
				return GetLastError(); // Unexpected error

			// 分配缓冲区存储依赖服务的数据
			lpDependencies = (LPENUM_SERVICE_STATUS) HeapAlloc( 
				GetProcessHeap(), HEAP_ZERO_MEMORY, dwBytesNeeded );

			if ( !lpDependencies )
				return GetLastError();

			__try {
				// 获得依赖服务
				if ( !EnumDependentServices( hService, SERVICE_ACTIVE, 
					lpDependencies, dwBytesNeeded, &dwBytesNeeded,
					&dwCount ) )
					return GetLastError();

				for ( i = 0; i < dwCount; i++ ) 
				{
					ess = *(lpDependencies + i);

					// 打开服务
					hDepService = OpenService( hSCM, ess.lpServiceName, 
						SERVICE_STOP | SERVICE_QUERY_STATUS );
					if ( !hDepService )
						return GetLastError();

					__try {
						// 结束服务
						if ( !ControlService( hDepService, 
							SERVICE_CONTROL_STOP,
							&ss ) )
							return GetLastError();

						// 等待服务结束
						while ( ss.dwCurrentState != SERVICE_STOPPED ) 
						{
							Sleep( ss.dwWaitHint );
							if ( !QueryServiceStatusEx( 
								hDepService, 
								SC_STATUS_PROCESS_INFO,
								(LPBYTE)&ssp, 
								sizeof(SERVICE_STATUS_PROCESS),
								&dwBytesNeeded ) )
								return GetLastError();

							if ( ss.dwCurrentState == SERVICE_STOPPED )
								break;

							if ( GetTickCount() - dwStartTime > dwTimeout )
								return ERROR_TIMEOUT;
						}

					} 
					__finally 
					{
						// 关闭服务
						CloseServiceHandle( hDepService );

					}
				}
			} 
			__finally 
			{
				// 释放内存
				HeapFree( GetProcessHeap(), 0, lpDependencies );
			}
		} 
	}

	// 所有的依赖服务已经结束，结束指定服务
	if ( !ControlService( hService, SERVICE_CONTROL_STOP, &ss ) )
		return GetLastError();
	while ( ss.dwCurrentState != SERVICE_STOPPED ) 
	{
		Sleep( ss.dwWaitHint );
		if ( !QueryServiceStatusEx( 
			hService, 
			SC_STATUS_PROCESS_INFO,
			(LPBYTE)&ssp, 
			sizeof(SERVICE_STATUS_PROCESS),
			&dwBytesNeeded ) )
			return GetLastError();

		if ( ss.dwCurrentState == SERVICE_STOPPED )
			break;

		if ( GetTickCount() - dwStartTime > dwTimeout )
			return ERROR_TIMEOUT;
	}
	return ERROR_SUCCESS;
}

/*************************************
* BOOL ControlSampleService(DWORD fdwControl) 
* 功能	向服务发送控制码
*
* 参数	DWORD fdwControl		控制码值
*		SCM	句柄，服务名直接使用全局变量
**************************************/
BOOL ControlSampleService(DWORD fdwControl) 
{ 
	SERVICE_STATUS ssStatus; 
	DWORD fdwAccess; 
	DWORD dwStartTickCount, dwWaitTime;

	// Access
	switch (fdwControl) 
	{ 
	case SERVICE_CONTROL_STOP: 
		fdwAccess = SERVICE_STOP; 
		break; 
	case SERVICE_CONTROL_PAUSE: 
	case SERVICE_CONTROL_CONTINUE: 
		fdwAccess = SERVICE_PAUSE_CONTINUE; 
		break; 
	case SERVICE_CONTROL_INTERROGATE: 
		fdwAccess = SERVICE_INTERROGATE; 
		break; 
	default: 
		fdwAccess = SERVICE_INTERROGATE; 
	} 

	// 打开服务
	schService = OpenService( 
		schSCManager,        // SCManager 句柄 
		szServiceName,		 // 服务名
		fdwAccess);          // 存取权限
	if (schService == NULL) 
	{
		printf("OpenService failed (%d)\n", GetLastError()); 
		return FALSE;
	}

	// 发送控制码
	if (! ControlService( 
		schService,   // 服务的句柄
		fdwControl,   // 控制码
		&ssStatus) )  // 状态
	{
		printf("ControlService failed (%d)\n", GetLastError()); 
		return FALSE;
	}

	// 显示状态
	printf("\nStatus of Sample_Srv: \n");
	printf("  Service Type: 0x%x\n", ssStatus.dwServiceType); 
	printf("  Current State: 0x%x\n", ssStatus.dwCurrentState); 
	printf("  Controls Accepted: 0x%x\n", 
		ssStatus.dwControlsAccepted); 
	printf("  Exit Code: %d\n", ssStatus.dwWin32ExitCode); 
	printf("  Service Specific Exit Code: %d\n", 
		ssStatus.dwServiceSpecificExitCode); 
	printf("  Check Point: %d\n", ssStatus.dwCheckPoint); 
	printf("  Wait Hint: %d\n", ssStatus.dwWaitHint); 

	return TRUE; 
}
