/* ************************************
*《精通Windows API》 
* 示例代码
* cur_mod_dir.c
* 4.3.4	获取当前目录、获取程序所在的目录、获取模块路径
**************************************/

/* 头文件　*/
#include <windows.h>
#include <stdio.h>

/* ************************************
* int main(void)
* 功能	演示使用设置获取当前路径
*		演示获取模块路径
**************************************/
int main(void)
{
	//用于存储当前路径
	CHAR szCurrentDirectory[MAX_PATH];
	//用于存储模块路径
	CHAR szMoudlePath[MAX_PATH];
	//Kernel32文件名与句柄
	LPSTR szKernel32 = "kernel32.dll";
	HMODULE hKernel32;
	//当前路径的长度，也用于判断获取是否成功
	DWORD dwCurDirPathLen;
	
	//获取进程当前目录
	dwCurDirPathLen = 
		GetCurrentDirectory(MAX_PATH,szCurrentDirectory);
	if(dwCurDirPathLen == 0)
	{
		printf("获取当前目录错误。\n"); 
		return 0;
	}
	printf("进程当前目录为 %s \n",szCurrentDirectory); 
	
	//将进程当前目录设置为“C:\”
	lstrcpy(szCurrentDirectory, "C:\\");
	if(!SetCurrentDirectory(szCurrentDirectory))
	{
		printf("设置当前目录错误。\n"); 
		return 0;
	}
	printf("已经设置当前目录为 %s \n",szCurrentDirectory); 

	//在当前目录下创建子目录“current_dir”
	//运行完成后C:盘下将出现文件夹“current_dir”
	CreateDirectory("current_dir", NULL);

	//再次获取系统当前目录
	dwCurDirPathLen = 
		GetCurrentDirectory(MAX_PATH,szCurrentDirectory);
	if(dwCurDirPathLen == 0)
	{
		printf("获取当前目录错误。\n"); 
		return 0;
	}
	printf("GetCurrentDirectory获取当前目录为 %s \n",
		szCurrentDirectory); 

	//使用NULL参数，获取本模块的路径。
	if(!GetModuleFileName(NULL,szMoudlePath,MAX_PATH))
	{
		printf("获取模块路径录错误。\n"); 
		return 0;
	}
	printf("本模块路径 %s \n",szMoudlePath);

	//获取Kernel32.dll的模块句柄。
	hKernel32 = LoadLibrary(szKernel32);

	//使用Kernel32.dll的模块句柄，获取其路径。
	if(!GetModuleFileName(hKernel32,szMoudlePath,MAX_PATH))
	{
		printf("获取模块路径错误。\n"); 
		return 0;
	}
	printf("kernel32模块路径 %s \n",szMoudlePath); 

	return 0;
}