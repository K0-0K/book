/* ************************************
*《精通Windows API》 
* 示例代码
* osinfo.c
* 10.1  系统信息
**************************************/
/* Windows 2k */
#define _WIN32_WINNT  0x0500
/* 头文件 */
#include <windows.h>
#include <Lmcons.h>
#include <stdio.h>
/* 函数声明 */
void ShowVersionInfo();
void ShowSystemInfo();
void GetFolders();
void GetNames();
void MouseSpeed();
/* ************************************
* int main()
* 功能	依次调用各示例函数
**************************************/
int main()
{
	ShowVersionInfo();
	ShowSystemInfo();
	GetFolders();
	GetNames();
	MouseSpeed();
}

/* ************************************
* void ShowVersionInfo()
* 功能	获取并显示系统版本信息
**************************************/
void ShowVersionInfo()
{
	OSVERSIONINFOEX ovex;
	CHAR szVersionInfo[1024];
	*szVersionInfo = '\x00';
	// 设置参数大小，调用并判断是否成功
	ovex.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	if(!GetVersionEx(&ovex))
	{
		printf("error %d\n",GetLastError());
		return;
	}
	// 判断版本
	if(ovex.dwMajorVersion==5)
	{
		if(ovex.dwMinorVersion==0)
			lstrcat(szVersionInfo,"Windows 2000 ");
		else if(ovex.dwMinorVersion==1)
			lstrcat(szVersionInfo,"Windows XP ");
		else if(ovex.dwMinorVersion==2)
			lstrcat(szVersionInfo,"Windows Server 2003 ");
	}
	else if(ovex.dwMajorVersion == 6)
		lstrcat(szVersionInfo,"Windows Vista ");
	else
		lstrcat(szVersionInfo,"Windows NT 4.0 或者其他 ");
	// 安装的SP，字符串
	lstrcat(szVersionInfo,ovex.szCSDVersion);
	// 判断wProductType，产品类型
	switch(ovex.wProductType)
	{
	case VER_NT_DOMAIN_CONTROLLER:
		lstrcat(szVersionInfo,"\n域控制器");
		break;
	case VER_NT_SERVER:
		lstrcat(szVersionInfo,"\n服务器");
		break;
	case VER_NT_WORKSTATION :
		lstrcat(szVersionInfo,"\n独立工作站");
		break;
	}
	// 判断wSuiteMask
	if(ovex.wSuiteMask & VER_SUITE_PERSONAL)
	{
		lstrcat(szVersionInfo,"\nWindows XP Home Edition");
	}
	if(ovex.wSuiteMask & VER_SUITE_SINGLEUSERTS)
	{
		lstrcat(szVersionInfo,"\n安装了终端服务，但只支持一个会话");
	}
	// wSuiteMask成员还可能是以下值的组合
	//VER_SUITE_BLADE
	//VER_SUITE_COMPUTE_SERVER
	//VER_SUITE_DATACENTER
	//VER_SUITE_ENTERPRISE
	//VER_SUITE_EMBEDDEDNT
	//VER_SUITE_PERSONAL
	//VER_SUITE_SINGLEUSERTS
	//VER_SUITE_SMALLBUSINESS
	//VER_SUITE_SMALLBUSINESS_RESTRICTED
	//VER_SUITE_STORAGE_SERVER
	//VER_SUITE_TERMINAL
	printf("%s\n",szVersionInfo);
}
/* ************************************
* void ShowSystemInfo()
* 功能	获取并显示硬件相关信息
**************************************/
void ShowSystemInfo()
{
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	printf("内存分页大小:0x%.8X，可用内存起始:0x%.8X，可用内存结束:0x%.8X，\n"
		"处理器个数:%d，处理器类型:",
		si.dwPageSize,
		si.lpMinimumApplicationAddress,
		si.lpMaximumApplicationAddress,
		si.dwNumberOfProcessors);

	switch (si.dwProcessorType)
	{
	case PROCESSOR_INTEL_386:
		printf("386");
		break;
	case PROCESSOR_INTEL_486:
		printf("486");
		break;
	case PROCESSOR_INTEL_PENTIUM:
		printf("pentium");
		printf(", Cpu Model 0x%.2X, Stepping 0x%.2X",
			(BYTE)(si.wProcessorRevision>>8),
			(BYTE)si.wProcessorRevision);
		break;
	}
	printf("\n处理器架构:");
	switch (si.wProcessorArchitecture)
	{
	case PROCESSOR_ARCHITECTURE_INTEL:
		printf("intel");
		printf(" CPU vendor is %d",si.wProcessorLevel);
		break;
	case PROCESSOR_ARCHITECTURE_IA64:
		printf("64 bits intel");
		break;
	case PROCESSOR_ARCHITECTURE_AMD64:
		printf("64 bits AMD");
		break;
	case PROCESSOR_ARCHITECTURE_UNKNOWN:
		printf("UNKNOWN");
		break;
	}
	printf("\n");
}
/* ************************************
*void GetFolders()
* 功能	获取系统目录等信息
**************************************/
void GetFolders()
{
	TCHAR szSystemDirectory[MAX_PATH];
	TCHAR szWindowsDirectory[MAX_PATH];

	GetSystemDirectory(szSystemDirectory,MAX_PATH);
	GetWindowsDirectory(szWindowsDirectory,MAX_PATH);
	printf("系统目录：\t%s\nWindows目录：\t%s\n",
		szSystemDirectory,
		szWindowsDirectory);
}
/* ************************************
* void GetNames()
* 功能	获取计算机名、用户名等信息
**************************************/
void GetNames()
{
	DWORD dwComputerNameLen = MAX_COMPUTERNAME_LENGTH+1;
	DWORD dwUserNameLen = UNLEN+1;
	TCHAR szComputerName[MAX_COMPUTERNAME_LENGTH+1];
	TCHAR szUserName[UNLEN + 1];
	if(!SetComputerName("My_Computer"))
	{
		printf("Set Error %d",GetLastError());
	}
	GetComputerName(szComputerName,&dwComputerNameLen);

	printf("计算机名：%s\n",szComputerName);

	//ComputerNameNetBIOS 
	//ComputerNameDnsHostname 
	//ComputerNameDnsFullyQualified 
	//ComputerNamePhysicalNetBIOS 
	//ComputerNamePhysicalDnsHostname 
	//ComputerNamePhysicalDnsDomain 
	//ComputerNamePhysicalDnsFullyQualified 
	dwComputerNameLen = MAX_COMPUTERNAME_LENGTH+1;
	GetComputerNameEx(ComputerNameDnsHostname,szComputerName,&dwComputerNameLen);
	printf("ComputerNameDnsHostname: %s\n",szComputerName);

	dwComputerNameLen = MAX_COMPUTERNAME_LENGTH+1;
	GetComputerNameEx(ComputerNamePhysicalNetBIOS,szComputerName,&dwComputerNameLen);
	printf("ComputerNamePhysicalNetBIOS: %s\n",szComputerName);	
	GetUserName(szUserName,&dwUserNameLen);
	printf("用户名：%s\n",szUserName);
}
/* ************************************
* void MouseSpeed()
* 功能	获取系统目录等信息
**************************************/
void MouseSpeed()
{

	BOOL fResult;
	int aMouseInfo[3];       // 保存数据信息的数组

	// 调用 SystemParametersInfo
	fResult = SystemParametersInfo(
		SPI_GETMOUSE,   // 获取鼠标信息
		0,              // 未使用
		&aMouseInfo,    // 用于保存鼠标信息
		0);             // 未使用

	// 把鼠标速度加倍 
	if( fResult )
	{
		aMouseInfo[2] = 2 * aMouseInfo[2]; 

		SystemParametersInfo(
			SPI_SETMOUSE,		// 设置鼠标信息
			0,                 // 未使用
			aMouseInfo,			// 鼠标信息
			SPIF_SENDCHANGE);  // 更新 win.ini 
	}
}