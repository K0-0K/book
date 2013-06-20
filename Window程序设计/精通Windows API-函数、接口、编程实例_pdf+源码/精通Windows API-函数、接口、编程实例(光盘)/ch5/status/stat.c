/* ************************************
*《精通Windows API》 
* 示例代码
* stat.c
* 5.5.2  获得当前系统内存使用情况
**************************************/

/* 头文件　*/
#include <windows.h>
#include <stdio.h>

/*************************************
* int main(void)
* 功能	获取内存使用情况
*
* 参数	未使用
**************************************/
int main(void)
{	
	//用于保存信息
	MEMORYSTATUSEX memstatusex;
	//设置结构大小
	memstatusex.dwLength = sizeof(memstatusex);
	//获取系统内存使用情况
	GlobalMemoryStatusEx(&memstatusex);
	//打印输入结果
	printf("TotalPhys\tAvailPhys\tTotalVirtual\tAvailVirtual\t"
		"TotalPageFile\tuAvailPageFile\tMemoryLoad\n"
		"%I64u\t%I64u\t%I64u\t%I64u\t%I64u\t%I64u\t%u%%",
		memstatusex.ullTotalPhys,memstatusex.ullAvailPhys,
		memstatusex.ullTotalVirtual,memstatusex.ullAvailVirtual,
		memstatusex.ullTotalPageFile,memstatusex.ullAvailPageFile,
		memstatusex.dwMemoryLoad);
	return 0;
}