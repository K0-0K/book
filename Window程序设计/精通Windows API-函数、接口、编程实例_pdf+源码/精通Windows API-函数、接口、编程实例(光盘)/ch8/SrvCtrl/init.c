/* ************************************
*《精通Windows API》 
* 示例代码
* init.c
* 6.6	服务
* 服务控制程序，安装，删除服务
**************************************/
/* 头文件　*/
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
/* 全局变量　*/
SC_HANDLE schService;
SC_HANDLE schSCManager;
LPTSTR szServiceName = TEXT("Sample_Srv");

/*************************************
* BOOL CreateSampleService(LPTSTR szPath, LPSTR szServiceName) 
* 功能	创建服务
*
* 参数
*		SC_HANDLE schSCManager，SCM句柄
*		LPTSTR szPath， 服务程序的路径
*		LPSTR szServiceName， 服务名
**************************************/
BOOL CreateSampleService( 
						 SC_HANDLE schSCManager,
						 LPTSTR szPath,
						 LPSTR szServiceName) 
{ 
	// 创建服务
	schService = CreateService( 
		schSCManager,				// SCM 句柄
		szServiceName,				// 服务名
		"Service sample",           // 显示的服务名
		SERVICE_ALL_ACCESS,			// 存取权限
		SERVICE_WIN32_OWN_PROCESS,	// 服务类别
		SERVICE_DEMAND_START,		// 启动类别
		SERVICE_ERROR_NORMAL,		// 错误控制类别
		szPath,						// 服务的可执行文件路径
		NULL,						// no load ordering group 
		NULL,						// no tag identifier 
		NULL,						// no dependencies 
		NULL,						// LocalSystem account 
		NULL);						// no password 

	if (schService == NULL) 
	{
		printf("CreateService failed (%d)\n", GetLastError()); 
		return FALSE;
	}
	else
	{
		printf("CreateService succeeded\n"); 
		CloseServiceHandle(schService); 
		return TRUE;
	}
}

/*************************************
* BOOL DeleteSampleService(LPTSTR szNameOfService) 
* 功能	删除服务
*
* 参数	LPTSTR szNameOfService	服务的名字
**************************************/
BOOL DeleteSampleService(LPTSTR szNameOfService) 
{ 
	schService = OpenService( 
		schSCManager,       // SCM 句柄 
		szNameOfService,	// 服务名
		DELETE);            // 可删除

	if (schService == NULL)
	{ 
		printf("OpenService failed (%d)\n", GetLastError()); 
		return FALSE;
	}
	// 删除服务
	if (! DeleteService(schService) ) 
	{
		printf("DeleteService failed (%d)\n", GetLastError()); 
		return FALSE;
	}
	else 
		printf("DeleteService succeeded\n"); 
	// 关闭句柄
	CloseServiceHandle(schService); 
	return TRUE;
}
/*************************************
* void main( int argc, TCHAR *argv[] )
* 功能	演示
*
* 参数	未使用
**************************************/
void main( int argc, TCHAR *argv[] ) 
{
	TCHAR szBinFilePath[MAX_PATH];
	PTCHAR pTemp;
	DWORD dwStopError;
	// 构造服务可执行程序的路径
	GetModuleFileName(NULL,szBinFilePath,MAX_PATH);
	pTemp = szBinFilePath+lstrlen(szBinFilePath);
	while(*--pTemp!='\\');
	lstrcpy(pTemp,TEXT("\\SplSrv.exe"));

	// 打开 SCM
	schSCManager = OpenSCManager( 
		NULL,                    // local machine 
		NULL,                    // ServicesActive database 
		SC_MANAGER_ALL_ACCESS);  // full access rights 

	if (NULL == schSCManager) 
		printf("OpenSCManager failed (%d)\n", GetLastError());

	// 创建服务
	CreateSampleService(schSCManager, szBinFilePath, szServiceName);
	// 启动服务
	StartSampleService(schSCManager,szServiceName);
	// 发送请求控制
	ControlSampleService(SERVICE_CONTROL_INTERROGATE);
	ControlSampleService(SERVICE_CONTROL_CONTINUE);
	// 停止服务
	dwStopError = StopService( schSCManager, szServiceName, 
		TRUE, 1000);

	if(ERROR_SUCCESS == dwStopError)
	{
		printf("Service Stoped\n"); 
	} 
	else
	{
		printf("Service stoped error (%u)\n",dwStopError); 
	}
	// 删除服务
	DeleteSampleService(szServiceName);
	CloseServiceHandle(schSCManager); 
}