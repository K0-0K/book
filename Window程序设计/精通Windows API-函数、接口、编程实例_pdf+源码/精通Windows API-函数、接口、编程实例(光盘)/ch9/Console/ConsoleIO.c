/* ************************************
*《精通Windows API》 
* 示例代码
* ConsoleIO.c
* 9.2  字符界面程序
**************************************/
/* 头文件 */
#include <windows.h> 
/* 函数声明 */
void ScrollScreenBuffer(HANDLE, INT); 
VOID HandleInput(LPSTR szInput);
/* 全局变量 */
HANDLE hStdout, hStdin; 
CONSOLE_SCREEN_BUFFER_INFO csbiInfo; 

/* ************************************
* int main(void)
* 功能	演示控制台的输入与输出
* 参数	无		返回值	无
**************************************/
void main(void) 
{
	// 提示

	LPSTR lpszPrompt = "Type a line and press Enter, 255 max \n q to quit "
		"\n i to Show console info\n c to Change backgroud color \n s to Change text color"
		"\n t to Find upper case character\n b to Call UseBlockIO \n u to Change conslos titile"
		"\n m to Show console event \n r to Change mode \n p to Clear up Console\n";

	CHAR chBuffer[256]; 
	DWORD cRead, cWritten, fdwMode, fdwOldMode; 
	WORD wOldColorAttrs; 
	//  STDIN 和 STDOUT 句柄
	hStdin = GetStdHandle(STD_INPUT_HANDLE); 
	hStdout = GetStdHandle(STD_OUTPUT_HANDLE); 
	if (hStdin == INVALID_HANDLE_VALUE || 
		hStdout == INVALID_HANDLE_VALUE) 
	{
		// 使用MessageBox报错
		MessageBox(NULL, "GetStdHandle", "Console Error", MB_OK);
		return;
	}

	// 当前字体颜色
	if (! GetConsoleScreenBufferInfo(hStdout, &csbiInfo)) 
	{
		MessageBox(NULL, "GetConsoleScreenBufferInfo", 
			"Console Error", MB_OK); 
		return;
	}
	wOldColorAttrs = csbiInfo.wAttributes; 

	// 设置字符颜色（红）和背景
	ChangeTextColor(hStdout, FOREGROUND_RED | FOREGROUND_INTENSITY);

	// 输出字符
	if (! WriteConsole( 
		hStdout,					// 输出句柄
		lpszPrompt,				// 输出字符串 
		lstrlen(lpszPrompt),	// 字符串长度 
		&cWritten,				// 返回实际输出的长度 
		NULL) )						// 非 overlapped
	{
		MessageBox(NULL, "WriteFile", "Console Error", MB_OK); 
		return; 
	}
	while (1) 
	{
		// 循环等待用户输入
		ZeroMemory(chBuffer,256);
		if (! ReadConsole( 
			hStdin,    // 输入控制台句柄
			chBuffer,  // 读入数据的缓冲区 
			255,       // 缓冲区大小
			&cRead,    // 实际读到的大小 
			NULL) )    // 非 overlapped 
			break; 
		// 处理用户的输入
		HandleInput(chBuffer);
	} 
}

/* ************************************
* VOID HandleInput(LPSTR szInput)
* 功能	处理用户的输入
* 参数	用户输入的字符串
**************************************/
VOID HandleInput(LPSTR szInput)
{
	switch(szInput[0])
	{
	case 'q':	// 退出进程
		ExitProcess(0);
	case 'i':		// 显示控制台信息 
		GetConsoleInfo(hStdout);
		break;
	case 'c':		// 设置界面背景颜色
		ChangeBackgroudColor(hStdout);
		break;
	case 's':		// 设置文件颜色
		ChangeTextColor(hStdout, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		break;
	case 't':		// 设置文件背景
		ChangeUpCaseTextBackgroudColor(hStdin, hStdout);
		break;
	case 'b':	// 使用Block IO
		UseBlockIO();
		break;
	case 'u':	// 设置控制台标题
		ChangeConsoleTitle(&szInput[2]);
		break;
	case 'm':	// 使用事件
		UseEvent();
		break;
	case 'r':		// 改变模式
		ChangeMode(hStdin,hStdout);
		break;
	case 'p':	// 清屏
		cls(hStdout);
		break;
	default:		// 其他，直接返回
		return;
	}
	return;
}