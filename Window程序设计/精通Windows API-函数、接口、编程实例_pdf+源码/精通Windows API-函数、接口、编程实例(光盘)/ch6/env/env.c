/* ************************************
*《精通Windows API》 
* 示例代码
* virtual.c
* 5.4.2  分配和释放可读可写的虚拟内存页面
**************************************/

/* 头文件　*/
#include <windows.h>
#include <stdio.h>


#include "../env_dll/env.h"


/*************************************
* int main(void)
* 功能	演示虚拟内存的使用
*
* 参数	未使用
**************************************/
int main(void)
{
	EnumEnvironmentVariables();
	ChangeEnviromentVariables("PATH","C:\\",VARIABLES_APPEND);
	EnumEnvironmentVariables();
	ChangeEnviromentVariables("PATH","C:\\",VARIABLES_NULL);
	EnumEnvironmentVariables();
	ChangeEnviromentVariables("XX","C:\\",VARIABLES_RESET);
	EnumEnvironmentVariables();
}