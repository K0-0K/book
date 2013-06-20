/* ************************************
*《精通Windows API》 
* 示例代码
* dir.c
* 4.3.3	创建目录
**************************************/

/* 头文件　*/
#include <windows.h>
#include <stdio.h>

/* ************************************
* int main(void)
* 功能	演示使用CreateDirectory创建目录
**************************************/
int main(void)
{
	//在程序的当前目录下创建“sub_dir”子目录
	LPSTR szDirPath = "sub_dir";
	if (!CreateDirectory(szDirPath, NULL)) 
	{ 
		printf("创建目录 %s 错误。\n",szDirPath); 
		return 1;
	}

	//在C盘下创建目录“example_dir”
	szDirPath = "C:\\example_dir";
	if (!CreateDirectory(szDirPath, NULL)) 
	{ 
		printf("创建目录 %s 错误。\n",szDirPath); 
		return 1;
	} 
	printf("成功\n"); 

	return 0;
}