/* ************************************
*《精通Windows API》 
* 示例代码
* Enum.c
* 6.7	进程状态信息
* 使用PSAPI 和 Tool helper库查看进程状态信息
**************************************/
/* 头文件　*/
#include <Windows.h>
#include <Psapi.h>
#include <Tlhelp32.h>
#include <stdio.h>
/* 预处理声明　*/
#pragma comment (lib, "psapi.lib")
/* 函数声明　*/
VOID WINAPI EnumProcess1();
VOID WINAPI EnumProcess2();
VOID ListProcessModules1( DWORD dwPID );
VOID ListProcessModules2( DWORD dwPID);
VOID PrintMemoryInfo( DWORD dwPID );
VOID ShowProcessMemoryInfo( DWORD dwPID );
VOID ListHeapInfo( DWORD dwPID );
VOID ListProcessThreads( DWORD dwPID );
VOID PrintError( LPTSTR msg );

/*************************************
* VOID WINAPI EnumProcess1()
* 功能	调用EnumProcesses遍历进程，
*		并调用ListProcessModules1函数和
*		ListProcessThreads函数列举模块和线程
*
* 无参数，无返回值
**************************************/
VOID WINAPI EnumProcess1()
{
	// 假设不超过1024个进程
	DWORD aProcesses[1024], cbNeeded, cProcesses;
	unsigned int i;
	// 调用EnumProcesses
	if ( !EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) )
		return;
	// 进程数
	cProcesses = cbNeeded / sizeof(DWORD);
	for ( i = 0; i < cProcesses; i++ )
	{
		// 显示进程信息
		printf( "\n\n**************************************************" );
		printf("\nPROCESS : %u\n\n",aProcesses[i]);
		printf( "\n****************************************************" );
		// 列举模块信息和线程信息
		ListProcessModules1( aProcesses[i] );
		ListProcessThreads( aProcesses[i] );
	}
}
/*************************************
* VOID WINAPI EnumProcess2()
* 功能	调用Process32First和Process32Next遍历进程，
*		并调用ListProcessModules2函数列举模块，
*		调用ShowProcessMemoryInfo函数显示内存使用情况
*
* 无参数，无返回值
**************************************/
VOID WINAPI EnumProcess2()
{
	HANDLE hProcessSnap;
	HANDLE hProcess;
	PROCESSENTRY32 pe32;
	DWORD dwPriorityClass;
	// Snapshot
	hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
	if( hProcessSnap == INVALID_HANDLE_VALUE )
	{
		PrintError( "CreateToolhelp32Snapshot (of processes)" );
		return ;
	}
	// 设置输入参数，结构的大小
	pe32.dwSize = sizeof( PROCESSENTRY32 );

	// 开始列举进程
	if( !Process32First( hProcessSnap, &pe32 ) )
	{
		PrintError( "Process32First" );  // 出错信息
		CloseHandle( hProcessSnap );
		return ;
	}
	do
	{
		// 打印进程名
		printf( "\n\n=====================================================" );
		printf( "\nPROCESS NAME:  %s", pe32.szExeFile );
		printf( "\n-----------------------------------------------------" );

		// 获取优先级
		dwPriorityClass = 0;
		hProcess = OpenProcess( PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID );
		if( hProcess == NULL )
			PrintError( "OpenProcess" );
		else
		{
			dwPriorityClass = GetPriorityClass( hProcess );
			if( !dwPriorityClass )
				PrintError( "GetPriorityClass" );
			CloseHandle( hProcess );
		}
		// 打印进程相关信息
		printf( "\n  process ID        = 0x%08X", pe32.th32ProcessID );
		printf( "\n  thread count      = %d",   pe32.cntThreads );
		printf( "\n  parent process ID = 0x%08X", pe32.th32ParentProcessID );
		printf( "\n  Priority Base     = %d", pe32.pcPriClassBase );
		if( dwPriorityClass )
			printf( "\n  Priority Class    = %d", dwPriorityClass );

		// 获取模块信息，显示内存使用情况
		ListProcessModules2( pe32.th32ProcessID );
		PrintMemoryInfo(pe32.th32ProcessID);
		ListHeapInfo(pe32.th32ProcessID);

	} while( Process32Next( hProcessSnap, &pe32 ) );

	CloseHandle( hProcessSnap );	//关闭句柄
	return ;
}

/*************************************
* VOID ListProcessModules1( DWORD dwPID )
* 功能	调用EnumProcessModules函数
*		列举和显示进程加载的模块
*
* 参数	DWORD dwPID	进程PID
**************************************/
VOID ListProcessModules1( DWORD dwPID )
{
	HMODULE hMods[1024];
	HANDLE hProcess;
	DWORD cbNeeded;
	unsigned int i;	

	printf( "\nListProcessModules1 Process ID %u\n", dwPID );

	// 打开进程，获得句柄
	hProcess = OpenProcess( PROCESS_QUERY_INFORMATION |
		PROCESS_VM_READ,
		FALSE, dwPID );
	if (NULL == hProcess)
		return;
	// 调用EnumProcessModules
	if( EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
	{
		for ( i = 0; i < (cbNeeded / sizeof(HMODULE)); i++ )
		{
			TCHAR szModName[MAX_PATH];
			// 获取获取的路径
			if ( GetModuleFileNameEx( hProcess, hMods[i], szModName,
				sizeof(szModName)/sizeof(TCHAR)))
			{
				// 打印
				printf( TEXT("\t%s (0x%08X)\n"), szModName, hMods[i] );
			}
		}
	}
	CloseHandle( hProcess );	// 关闭进程句柄
}

/*************************************
* VOID ListProcessModules2( DWORD dwPID )
* 功能	调用Module32First和Module32Next函数
*		列举和显示进程加载的模块
*
* 参数	DWORD dwPID	进程PID
**************************************/
VOID ListProcessModules2( DWORD dwPID)
{
	HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
	MODULEENTRY32 me32;
	printf( "\nListProcessModules2 Process ID %u\n", dwPID );
	// Snapshot
	hModuleSnap = CreateToolhelp32Snapshot( TH32CS_SNAPMODULE, dwPID );
	if( hModuleSnap == INVALID_HANDLE_VALUE )
	{
		PrintError( "CreateToolhelp32Snapshot (of modules)" );
		return ;
	}
	// 设置输入参数，结构的大小
	me32.dwSize = sizeof( MODULEENTRY32 );
	// 开始获取模块信息
	if( !Module32First( hModuleSnap, &me32 ) )
	{
		PrintError( "Module32First" );  // Show cause of failure
		CloseHandle( hModuleSnap );     // Must clean up the snapshot object!
		return ;
	}
	do
	{
		printf( "\n\n     MODULE NAME:     %s",             me32.szModule );
		printf( "\n     executable     = %s",             me32.szExePath );
		printf( "\n     process ID     = 0x%08X",         me32.th32ProcessID );
		printf( "\n     ref count (g)  =     0x%04X",     me32.GlblcntUsage );
		printf( "\n     ref count (p)  =     0x%04X",     me32.ProccntUsage );
		printf( "\n     base address   = 0x%08X", (DWORD) me32.modBaseAddr );
		printf( "\n     base size      = %d",             me32.modBaseSize );

	} while( Module32Next( hModuleSnap, &me32 ) );
	CloseHandle( hModuleSnap );	// 关闭句柄
	return ;
}

/*************************************
* VOID PrintMemoryInfo( DWORD dwPID )
* 功能	显示进程的内存使用情况
*
* 参数	DWORD dwPID	进程PID
**************************************/
VOID PrintMemoryInfo( DWORD dwPID )
{
	HANDLE hProcess;
	PROCESS_MEMORY_COUNTERS pmc;

	printf( "\nProcess ID: %u\n", dwPID );

	hProcess = OpenProcess(  PROCESS_QUERY_INFORMATION |
		PROCESS_VM_READ,
		FALSE, dwPID );
	if (NULL == hProcess)
		return;

	if ( GetProcessMemoryInfo( hProcess, &pmc, sizeof(pmc)) )
	{
		printf( "\tPageFaultCount: 0x%08X\n", pmc.PageFaultCount );
		printf( "\tPeakWorkingSetSize: 0x%08X\n", 
			pmc.PeakWorkingSetSize );
		printf( "\tWorkingSetSize: 0x%08X\n", pmc.WorkingSetSize );
		printf( "\tQuotaPeakPagedPoolUsage: 0x%08X\n", 
			pmc.QuotaPeakPagedPoolUsage );
		printf( "\tQuotaPagedPoolUsage: 0x%08X\n", 
			pmc.QuotaPagedPoolUsage );
		printf( "\tQuotaPeakNonPagedPoolUsage: 0x%08X\n", 
			pmc.QuotaPeakNonPagedPoolUsage );
		printf( "\tQuotaNonPagedPoolUsage: 0x%08X\n", 
			pmc.QuotaNonPagedPoolUsage );
		printf( "\tPagefileUsage: 0x%08X\n", pmc.PagefileUsage ); 
		printf( "\tPeakPagefileUsage: 0x%08X\n", 
			pmc.PeakPagefileUsage );
	}
	CloseHandle( hProcess );
}

/*************************************
* VOID ListHeapInfo( DWORD dwPID )
* 功能	显示进程的堆分配情况
*
* 参数	DWORD dwPID	进程PID
**************************************/
VOID ListHeapInfo( DWORD dwPID )
{
	HEAPLIST32 hl;
	HEAPENTRY32 he;
	HANDLE hSnapshot = INVALID_HANDLE_VALUE;

	printf( "\\ListHeapInfo Process ID %u\n", dwPID );

	//  Snapshot
	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPHEAPLIST , dwPID ); 
	if( hSnapshot == INVALID_HANDLE_VALUE ) 
	{ 
		PrintError( "CreateToolhelp32Snapshot (of heaplist)" ); 
		return ; 
	}
	// 设置输入参数，结构的大小
	hl.dwSize = sizeof( HEAPLIST32 ); 

	// 开始获取信息
	if( !Heap32ListFirst( hSnapshot, &hl ) ) 
	{ 
		PrintError( "Heap32ListFirst" );  
		CloseHandle( hSnapshot );     
		return ; 
	} 
	do 
	{ 
		printf( "\n\tHeap ID     =%u", hl.th32HeapID ); 
		printf( "\tHeap Flags     = %u", hl.dwFlags ); 
		he.dwSize = sizeof(HEAPENTRY32);
		if( !Heap32First(&he,dwPID,hl.th32HeapID) ) 
		{ 
			PrintError( "Heap32First" );	// 出错
			CloseHandle( hSnapshot );     
			return ; 
		}
		do 
		{ 
			// 显示信息
			printf( "\n\t\t Heap Address\t= %u",he.dwAddress ); 
			printf( "\t Heap Size\t= %u",he.dwBlockSize); 
			printf( "\t Heap Flags\t= %u",he.dwFlags);
			printf( "\t Heap Handle\t= %u",he.hHandle);
		} while( Heap32Next(&he )); 

	} while( Heap32ListNext( hSnapshot, &hl ) ); 
	CloseHandle( hSnapshot );	// 关闭句柄
	return ; 
}

/*************************************
* VOID ListProcessThreads( DWORD dwPID )
* 功能	调用Thread32First和Thread32Next
*		显示一个进程的线程
*
* 参数	DWORD dwPID	进程PID
**************************************/
VOID ListProcessThreads( DWORD dwPID )
{
	HANDLE hThreadSnap = INVALID_HANDLE_VALUE; 
	THREADENTRY32 te32; 

	printf( "\\ListProcessThreads Process ID %u\n", dwPID );

	// Snapshot
	hThreadSnap = CreateToolhelp32Snapshot( TH32CS_SNAPTHREAD, 0 ); 
	if( hThreadSnap == INVALID_HANDLE_VALUE ) 
		return ; 

	// 设置输入参数，结构的大小
	te32.dwSize = sizeof(THREADENTRY32 ); 

	// 开始获取信息
	if( !Thread32First( hThreadSnap, &te32 ) ) 
	{
		PrintError( "Thread32First" );  // Show cause of failure
		CloseHandle( hThreadSnap );     // Must clean up the snapshot object!
		return ;
	}
	do 
	{ 
		if( te32.th32OwnerProcessID == dwPID )
		{
			// 显示相关信息
			printf( "\n   THREAD ID = 0x%08X", te32.th32ThreadID ); 
			printf( "\t   base priority = %d", te32.tpBasePri ); 
			printf( "\t   delta priority = %d", te32.tpDeltaPri ); 
		}
	} while( Thread32Next(hThreadSnap, &te32 ) ); 

	CloseHandle( hThreadSnap );
	return ;

}

// 打印出错信息
VOID PrintError( LPTSTR msg )
{
	DWORD eNum;
	TCHAR sysMsg[256];
	TCHAR* p;

	eNum = GetLastError( );
	FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, eNum,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
		sysMsg, 256, NULL );
	p = sysMsg;
	while( ( *p > 31 ) || ( *p == 9 ) )
		++p;
	do { *p-- = 0; } while( ( p >= sysMsg ) &&
		( ( *p == '.' ) || ( *p < 33 ) ) );
	printf( "\n  WARNING: %s failed with error %d (%s)", msg, eNum, sysMsg );
}

void main()
{
	printf("EnumProcess1 \n");
	EnumProcess1();
	printf("\n\n\nEnumProcess2 \n");
	EnumProcess2();
}