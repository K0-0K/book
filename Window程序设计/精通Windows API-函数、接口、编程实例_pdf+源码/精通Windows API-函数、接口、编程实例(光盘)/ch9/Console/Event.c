/* ************************************
*《精通Windows API》 
* 示例代码
* Event.c
* 9.2  字符界面程序
**************************************/
/* 头文件 */
#include <windows.h>
/* 函数声明 */
VOID KeyEventProc(KEY_EVENT_RECORD); 
VOID MouseEventProc(MOUSE_EVENT_RECORD); 
VOID ResizeEventProc(WINDOW_BUFFER_SIZE_RECORD); 

/* ************************************
* DWORD UseEvent(VOID) 
* 功能	使用事件进行控制台操作
**************************************/
DWORD UseEvent(VOID) 
{ 
	CHAR chBuffer[256]; 
	DWORD cRead;
	HANDLE hStdin; 
	DWORD cNumRead, fdwMode, fdwSaveOldMode, i; 
	INPUT_RECORD irInBuf[128]; 

	// 获取标准输入句柄
	hStdin = GetStdHandle(STD_INPUT_HANDLE); 
	if (hStdin == INVALID_HANDLE_VALUE) 
		MyErrorExit("GetStdHandle"); 

	// 保存当前的控制台模式
	if (! GetConsoleMode(hStdin, &fdwSaveOldMode) ) 
		MyErrorExit("GetConsoleMode"); 

	// 使能窗口鼠标输入事件
	fdwMode = ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT; 
	if (! SetConsoleMode(hStdin, fdwMode) ) 
		MyErrorExit("SetConsoleMode"); 

	// 循环读取输入
	while (1) 
	{ 

		// 等待事件
		if (! ReadConsoleInput( 
			hStdin,      // 输入句柄
			irInBuf,     // 保存输入的缓冲区 
			128,         // 缓冲区大小 
			&cNumRead) ) // 实际读取的大小 
			MyErrorExit("ReadConsoleInput"); 

		// 显示事件
		for (i = 0; i < cNumRead; i++) 
		{
			switch(irInBuf[i].EventType) 
			{ 
			case KEY_EVENT: // 键盘输入
				KeyEventProc(irInBuf[i].Event.KeyEvent); 
				break; 

			case MOUSE_EVENT: // 鼠标输入
				MouseEventProc(irInBuf[i].Event.MouseEvent); 
				break; 

			case WINDOW_BUFFER_SIZE_EVENT: // Resize
				ResizeEventProc( 
					irInBuf[i].Event.WindowBufferSizeEvent); 
				break; 

			case FOCUS_EVENT:  // focus 事件 

			case MENU_EVENT:   // menu 事件 
				break; 

			default: 
				MyErrorExit("unknown event type"); 
				break; 
			} 
		}
	} 
	return 0; 
}

/* ************************************
* VOID KeyEventProc(KEY_EVENT_RECORD ker)
* 功能	使用对话框显示键盘输入事件
* 参数	KEY_EVENT_RECORD ker  键盘事件
**************************************/
VOID KeyEventProc(KEY_EVENT_RECORD ker)
{
	CHAR szMsg[256];
	wsprintf(szMsg,"KEY_EVENT_RECORD\n Char = %c",
		ker.uChar);
	MessageBox(NULL,szMsg,"EVENT",MB_OK);
}

/* ************************************
* VOID MouseEventProc(MOUSE_EVENT_RECORD mer)
* 功能	使用对话框显示鼠标事件
* 参数	MOUSE_EVENT_RECORD mer 鼠标事件
**************************************/
VOID MouseEventProc(MOUSE_EVENT_RECORD mer)
{
	CHAR szMsg[256];
	wsprintf(szMsg,"MOUSE_EVENT_RECORD\n button state: %d\nmouse position X=%u,Y=%u",
		mer.dwButtonState,mer.dwMousePosition.X,mer.dwMousePosition.Y);
	MessageBox(NULL,szMsg,"EVENT",MB_OK);
	if(IDOK == MessageBox(NULL,"Exit?","EVENT",MB_OKCANCEL))
	{
		ExitProcess(0);
	}
} 

/* ************************************
* VOID ResizeEventProc(WINDOW_BUFFER_SIZE_RECORD wbsr)
* 功能	重置大小事件
* 参数	WINDOW_BUFFER_SIZE_RECORD wbsr) 事件
**************************************/
VOID ResizeEventProc(WINDOW_BUFFER_SIZE_RECORD wbsr)
{
	CHAR szMsg[256];
	wsprintf(szMsg,"WINDOW_BUFFER_SIZE_RECORD\nX=%u,Y=%u",
		wbsr.dwSize.X,wbsr.dwSize.Y);
	MessageBox(NULL,szMsg,"EVENT",MB_OK);
} 