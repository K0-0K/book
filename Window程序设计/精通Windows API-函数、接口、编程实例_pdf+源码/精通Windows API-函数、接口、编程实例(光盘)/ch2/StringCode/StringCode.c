/* ************************************
*《精通Windows API》 
* 示例代码
* StringCode.c
* 2.4	Unicode和多字节
**************************************/

/* 预处理　*/
/* 头文件　*/
#include <windows.h>

/* ************************************
* 功能	Unicode与多字节编码演示
**************************************/
int WINAPI WinMain(
			HINSTANCE hInstance,
			HINSTANCE hPrevInstance,
			LPSTR lpCmdLine,
			int nCmdShow
			)
{
    //定义LPWSTR类型的宽字符串
	LPWSTR szUnicode = L"This is a Unicode String;";
    //定义LPSTR类型的窄字符串
    LPSTR szMutliByte = "This is not a Unicode String;";
    //定义LPTSTR类型的自适就字符串
    LPTSTR szString = TEXT("This string is Unicode or not depends on the option.");

    //使用W版本的API函数，以宽字符串为参数。
    MessageBoxW(NULL,szUnicode,L"<字符编码1>",MB_OK);
    //使用A版本的API函数，以窄字符串为参数。
    MessageBoxA(NULL,szMutliByte,"<字符编码2>",MB_OK);
    //使用自适用的API函数，采用相适应的字符串类型为参数。
    MessageBox(NULL,szString,TEXT("<字符编码3>"),MB_OK);

	return 0;
}