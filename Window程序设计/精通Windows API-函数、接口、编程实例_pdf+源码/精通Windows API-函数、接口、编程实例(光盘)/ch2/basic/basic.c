/* ************************************
*《精通Windows API》 
* 示例代码
* basic.c
* 2.2  Windows API的功能分类
**************************************/

/* 头文件　*/
#include <windows.h>

/* ************************************
* 功能	获取系统目录信息，并存储到文件中
**************************************/
int main(int argc, TCHAR argv[])
{
    //文件句柄
    HANDLE hFile;

    DWORD dwWritten;
    //字符数组，用于存储系统目录
    TCHAR szSystemDir[MAX_PATH];
    //获取系统目录
    GetSystemDirectory(szSystemDir,MAX_PATH);

    //创建文件systemroot.txt
    hFile = CreateFile("systemroot.txt",
        GENERIC_WRITE,
        0,NULL,CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL);
    //判断文件是否创建成功
    if(hFile != INVALID_HANDLE_VALUE)
    {
        //将系统目录系统信息写入文件
        if(!WriteFile(hFile,szSystemDir,lstrlen(szSystemDir),&dwWritten,NULL))
        {
            return GetLastError();
        }
    }
    //关闭文件，返回。
    CloseHandle(hFile);
    return 0;
}