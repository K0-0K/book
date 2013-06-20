/* ************************************
*《精通Windows API》
* 示例代码
* setup.c
* 15.3 setup.exe
**************************************/
/* 头文件 */
#include <Windows.h>
#include <Setupapi.h>
#include <shlobj.h>
/* 库 */
#pragma comment (lib, "shell32.lib")
#pragma comment (lib, "Setupapi.lib")

/*************************************
* VOID GetSourceDirectory(LPSTR szPath)
* 功能 获得当前路径
* szPath，返回路径
**************************************/
VOID GetSourceDirectory(LPSTR szPath)
{
	int i;

	GetModuleFileName(NULL,szPath,MAX_PATH);

	i=strlen(szPath);
	while ((i>0)&&(szPath[i-1]!='\\'))
	{
		szPath[--i]=0;
	}
}

/*************************************
* WinMain
* 功能 调用相关Setup API进行安装
**************************************/
INT WinMain(
			HINSTANCE hInstance,
			HINSTANCE hPrevInstance,
			LPSTR lpCmdLine,
			int nCmdShow
			)
{
	HINF hInf; // INF文件句柄
	CHAR szSrcPath[MAX_PATH];// 源路径
	CHAR szDisPath[MAX_PATH];// 目的路径
	BOOL bResult;
	PVOID pContext;
	// 与本程序在同一目录下的Setup.inf
	GetSourceDirectory(szSrcPath);
	lstrcat(szSrcPath,"setup.inf");
	// 打开 inf 文件
	hInf = SetupOpenInfFile(szSrcPath, NULL, INF_STYLE_WIN4, NULL);
	// 是否成功
	if (hInf == INVALID_HANDLE_VALUE)
	{
		MessageBox(NULL,
			"Error: Could not open the INF file.",
			"ERROR",
			MB_OK|MB_ICONERROR);
		return FALSE;
	}
	// 获得Program Files的路径
	SHGetSpecialFolderPath(NULL, 
		szDisPath, CSIDL_PROGRAM_FILES , FALSE);
	// 构造目的路径
	lstrcat(szDisPath,"\\MyInstall");

	// 给inf配置文件中的路径ID赋值，使用路径替换路径ID
	bResult = SetupSetDirectoryId(hInf, 32768, szDisPath);
	if (!bResult)
	{
		MessageBox(NULL,
			"Error: Could not associate a directory ID with the destination directory.",
			"ERROR",
			MB_OK|MB_ICONERROR);
		SetupCloseInfFile(hInf);
		return FALSE;
	}

	// 设置默认callback函数的参数
	pContext=SetupInitDefaultQueueCallback(NULL);

	// 进行安装
	bResult=SetupInstallFromInfSection(
		NULL, // 父窗口句柄
		hInf, // INF文件句柄
		"Install", // INF文件中，配置了安装信息的节名
		SPINST_FILES | SPINST_REGISTRY , // 安装标志
		NULL, // 安装键值
		NULL, // 源文件和路径，可以在INF文件中配置
		0, // 复制时的动作
		(PSP_FILE_CALLBACK)SetupDefaultQueueCallback, // 回调函数
		pContext, // 回调函数的参数
		NULL, // 设备信息
		NULL // 设备信息
		);
	// 安装是否成功
	if (!bResult)
	{
		// 失败，输出错误信息
		MessageBox(NULL,
			"SetupInstallFromInfSection",
			"ERROR",
			MB_OK|MB_ICONERROR);
		//关闭
		SetupTermDefaultQueueCallback(pContext);
		SetupCloseInfFile(hInf);
		return FALSE;
	}

	// 关闭
	SetupTermDefaultQueueCallback(pContext);
	SetupCloseInfFile(hInf);

	return TRUE;
}