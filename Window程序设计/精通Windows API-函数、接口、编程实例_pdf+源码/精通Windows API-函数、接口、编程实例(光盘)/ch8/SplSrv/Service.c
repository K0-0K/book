/* ************************************
*《精通Windows API》
* 示例代码
* Service.c
* 6.6	服务
* 编译生成SplSrv.exe 作为服务启动。
**************************************/
/* 头文件　*/
#include <windows.h>
/* 全局变量　*/
SERVICE_STATUS          SplSrvServiceStatus; 
SERVICE_STATUS_HANDLE   SplSrvServiceStatusHandle; 
/* 函数声明　*/
VOID SvcDebugOut(LPSTR String, DWORD Status);
VOID WINAPI SplSrvServiceCtrlHandler (DWORD opcode); 
VOID WINAPI SplSrvServiceStart (DWORD argc, LPTSTR *argv); 
DWORD SplSrvServiceInitialization (DWORD argc, LPTSTR *argv, 
								   DWORD *specificError); 

/*************************************
* VOID WINAPI SplSrvServiceStart (DWORD argc, LPTSTR *argv)
* 功能	服务启动函数
*
* 参数	未使用
**************************************/
VOID WINAPI SplSrvServiceStart (DWORD argc, LPTSTR *argv) 
{ 
	DWORD status; 
	DWORD specificError; 
	// 填充SERVICE_STATUS 结构
	SplSrvServiceStatus.dwServiceType        = SERVICE_WIN32; 
	SplSrvServiceStatus.dwCurrentState       
		= SERVICE_START_PENDING;		// 服务在运行
	SplSrvServiceStatus.dwControlsAccepted   
		= SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE; 
	SplSrvServiceStatus.dwWin32ExitCode      = 0; 
	SplSrvServiceStatus.dwServiceSpecificExitCode = 0; 
	SplSrvServiceStatus.dwCheckPoint         = 0; 
	SplSrvServiceStatus.dwWaitHint           = 0; 
	// 注册服务控制请求处理例程
	SplSrvServiceStatusHandle = RegisterServiceCtrlHandler( 
		"Sample_Srv",	// 服务名，在创建服务时使用了
		// SERVICE_WIN32_OWN_PROCESS，因此本参数被忽略。
		SplSrvServiceCtrlHandler);	// 控制请求处理例程，函数名

	if (SplSrvServiceStatusHandle == (SERVICE_STATUS_HANDLE)0) 
	{ 
		SvcDebugOut(" [SPLSRV_SERVICE] RegisterServiceCtrlHandler "
			"failed %d\n", GetLastError()); 
		return; 
	}  
	// 初始化工作，本示例未使用，函数为空
	status = SplSrvServiceInitialization(argc,argv, &specificError);  
	// 初始化出错，用户自行修改
	if (status != NO_ERROR) 
	{ 
		SplSrvServiceStatus.dwCurrentState       = SERVICE_STOPPED; 
		SplSrvServiceStatus.dwCheckPoint         = 0; 
		SplSrvServiceStatus.dwWaitHint           = 0; 
		SplSrvServiceStatus.dwWin32ExitCode      = status; 
		SplSrvServiceStatus.dwServiceSpecificExitCode = specificError; 

		SetServiceStatus (SplSrvServiceStatusHandle, &SplSrvServiceStatus); 
		return; 
	} 
	// 初始化完成，设置运行状态
	SplSrvServiceStatus.dwCurrentState       = SERVICE_RUNNING; 
	SplSrvServiceStatus.dwCheckPoint         = 0; 
	SplSrvServiceStatus.dwWaitHint           = 0; 

	if (!SetServiceStatus (SplSrvServiceStatusHandle, &SplSrvServiceStatus)) 
	{ 
		status = GetLastError(); 
		SvcDebugOut(" [SPLSRV_SERVICE] SetServiceStatus error %ld\n",status); 
	} 
	// 用户自行修改，用于完成服务的工作
	SvcDebugOut(" [SPLSRV_SERVICE] Returning the Main Thread \n",0); 

	return; 
} 


/*************************************
* DWORD SplSrvServiceInitialization(DWORD   argc, 
*	LPTSTR  *argv, 
*	DWORD *specificError)
* 功能	初始化，这里未进行任何工作，留待读者修改
*
* 参数	
**************************************/
DWORD SplSrvServiceInitialization(DWORD   argc, 
								  LPTSTR  *argv, 
								  DWORD *specificError) 
{ 
	return(0); 
}

/*************************************
* VOID WINAPI SplSrvServiceCtrlHandler (DWORD Opcode)
* 功能	服务控制请求的处理函数，与ControlService函数配合。
*
* 参数	服务控制码
**************************************/
VOID WINAPI SplSrvServiceCtrlHandler (DWORD Opcode) 
{ 
	DWORD status; 
	switch(Opcode) 
	{ 
	case SERVICE_CONTROL_PAUSE: 
		// 完成相关功能
		SplSrvServiceStatus.dwCurrentState = SERVICE_PAUSED; 
		break; 
	case SERVICE_CONTROL_CONTINUE: 
		// 完成相关功能
		SplSrvServiceStatus.dwCurrentState = SERVICE_RUNNING; 
		break; 
	case SERVICE_CONTROL_STOP: 
		// 完成相关功能
		SplSrvServiceStatus.dwWin32ExitCode = 0; 
		SplSrvServiceStatus.dwCurrentState  = SERVICE_STOPPED; 
		SplSrvServiceStatus.dwCheckPoint    = 0; 
		SplSrvServiceStatus.dwWaitHint      = 0; 

		if (!SetServiceStatus (SplSrvServiceStatusHandle, 
			&SplSrvServiceStatus))
		{ 
			status = GetLastError(); 
			SvcDebugOut(" [SPLSRV_SERVICE] SetServiceStatus error %ld\n", 
				status); 
		}  
		SvcDebugOut(" [SPLSRV_SERVICE] Leaving SplSrvService \n",0); 
		return; 
	case SERVICE_CONTROL_INTERROGATE: 
		// 收到此请求后发出声响，演示服务控制请求的处理过程，读者可自行修改
		MessageBeep(MB_OK);
		break; 
	default: 
		SvcDebugOut(" [SPLSRV_SERVICE] Unrecognized opcode %ld\n", 
			Opcode); 
	} 
	// 当前状态
	if (!SetServiceStatus (SplSrvServiceStatusHandle,  &SplSrvServiceStatus)) 
	{ 
		status = GetLastError(); 
		SvcDebugOut(" [SPLSRV_SERVICE] SetServiceStatus error %ld\n", 
			status); 
	} 
	return; 
}
/*************************************
* void main( ) 
* 功能	程序入口函数，注册服务启动函数等。
*
* 参数	服务控制码
**************************************/
void main( ) 
{ 
	// 设置SERVICE_TABLE_ENTRY 数据结构，以NULL 结构结束，
	// 作为StartServiceCtrlDispatcher 函数的参数。
	SERVICE_TABLE_ENTRY   DispatchTable[] = 
	{ 
		{ "Sample_Srv", (LPSERVICE_MAIN_FUNCTION) SplSrvServiceStart }, 
		{ NULL, NULL } 
	}; 
	if (!StartServiceCtrlDispatcher( DispatchTable)) 
	{ 
		SvcDebugOut(" [SPLSRV_SERVICE] StartServiceCtrlDispatcher (%d)\n", 
			GetLastError()); 
	} 
} 
/*************************************
* VOID SvcDebugOut(LPSTR String, DWORD Status) 
* 功能	显示信息给调试器。
*
* 参数	LPSTR String	消息字符串
*		DWORD Status	状态
**************************************/
VOID SvcDebugOut(LPSTR String, DWORD Status) 
{ 
	CHAR  Buffer[1024]; 
	if (strlen(String) < 1000) 
	{ 
		wsprintf(Buffer, String, Status); 
		OutputDebugString(Buffer); 
	} 
}
