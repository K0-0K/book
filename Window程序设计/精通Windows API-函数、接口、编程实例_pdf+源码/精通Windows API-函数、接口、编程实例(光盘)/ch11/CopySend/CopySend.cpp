/* ************************************
*《精通Windows API》 
* 示例代码
* CopySend.c
* 11.4 通过WM_COPYDATA进程间通信
**************************************/
/* 头文件 */
#include <windows.h> 
#include <commctrl.h>
#include "resource.h"
/* 全局变量 */
HINSTANCE hinst;
HWND hwnd, hwndEdit;
LPSTR lpszCopy =  "you can input any text\n then click \'Send\' Item at Menu"; 
/* 函数声明 */
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int); 
LRESULT CALLBACK MainWndProc(HWND, UINT, WPARAM, LPARAM); 

/* ************************************
* WinMain
**************************************/
int WINAPI WinMain(
				   HINSTANCE hinstance, 
				   HINSTANCE hPrevInstance, 
				   LPSTR lpCmdLine,
				   int nCmdShow) 
{ 
	MSG msg; 
	WNDCLASSEX wcx; 

	hinst = hinstance;	// 保存应用程序实例

	// 填充WNDCLASSEX
	wcx.cbSize = sizeof(wcx);      
	wcx.style = CS_HREDRAW | CS_VREDRAW;                   
	wcx.lpfnWndProc = MainWndProc;		// 消息处理函数
	wcx.cbClsExtra = 0;        
	wcx.cbWndExtra = 0;             
	wcx.hInstance = hinstance;   
	wcx.hIcon = LoadIcon(NULL, 	IDI_APPLICATION);          
	wcx.hCursor = LoadCursor(NULL, IDC_ARROW);                 
	wcx.hbrBackground = (HBRUSH)GetStockObject( WHITE_BRUSH);               
	wcx.lpszMenuName =  MAKEINTRESOURCE(IDR_MENU_COMMAND);   // 菜单
	wcx.lpszClassName = "copydata";		// 窗口类名
	wcx.hIconSm = NULL; 
	// 注册窗口类
	if(RegisterClassEx(&wcx) == 0)
	{
		return 0;
	}
	// 创建窗口
	hwnd = CreateWindow( 
		"copydata",        
		"Sample",           
		WS_TILEDWINDOW, 
		CW_USEDEFAULT, CW_USEDEFAULT, 500, 	400,    
		(HWND) NULL,        
		(HMENU) NULL,      
		hinstance,         
		(LPVOID) NULL);    
	if (!hwnd) 
		return 0; 

	// 显示、刷新窗口
	ShowWindow(hwnd, nCmdShow); 
	UpdateWindow(hwnd); 
	// 消息循环

	BOOL fGotMessage;
	while ((fGotMessage = GetMessage(&msg, (HWND) NULL, 0, 0)) != 0 && fGotMessage != -1) 
	{ 
		TranslateMessage(&msg); 
		DispatchMessage(&msg); 
	} 
	return msg.wParam; 
	UNREFERENCED_PARAMETER(lpCmdLine); 
} 

/* ************************************
* 消息处理函数
**************************************/
LONG APIENTRY MainWndProc(HWND hwnd,          
						  UINT message,           
						  WPARAM wParam,         
						  LPARAM lParam)          
{ 
	switch (message) 
	{ 
	case WM_CREATE:
		{
			// 在主窗口创建时，创建一个EDIT控件，用于编辑需要复制的内容
			RECT rectMain;
			GetClientRect(hwnd,&rectMain);
			hwndEdit = CreateWindow("EDIT",    
				NULL,       
				WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_BORDER |
				ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL, 
				0., 0, rectMain.right, rectMain.bottom, 
				hwnd,  	NULL,
				(HINSTANCE) GetWindowLong(hwnd, GWL_HINSTANCE), 
				NULL);     
			if(hwndEdit == NULL)
			{
				MessageBox(hwnd, "Create Window Error","ERROR",MB_OK);
				ExitProcess(0);
			}
			// 设置EDIT控件的内容
			SendMessage(hwndEdit, WM_SETTEXT, 0, (LPARAM) lpszCopy); 
			break;
		}
	case WM_COMMAND:		// 菜单输入
		{
			// 如果通过菜单点击“send”按钮
			if (LOWORD(wParam) == ID_COMMAND_SEND) 
			{
				// 获取EDIT控件中的文本
				DWORD dwTextLenth = GetWindowTextLength(hwndEdit);
				LPSTR szText = (LPSTR)HeapAlloc(GetProcessHeap(),HEAP_NO_SERIALIZE,dwTextLenth);
				GetWindowText(hwndEdit,szText,dwTextLenth);
				// 构造 COPYDATASTRUCT
				COPYDATASTRUCT cds;
				HWND hwnd2 = FindWindow("copydata2","RECV COPY DATA");
				if(hwnd2 == NULL)
				{
					MessageBox(hwnd,"can not find window 2","ERROR",MB_OK);
					break;
				}
				cds.dwData = 0x12345678;			// 自定义的数据
				cds.cbData = dwTextLenth;		// lpData的长度
				cds.lpData = szText;				// 需要复制的数据
				// 发送消息，COPYDATASTRUCT结构的指针做为lParam
				SendMessage(    
					hwnd2,									// 目的窗口
					(UINT) WM_COPYDATA,		// 消息 ID
					(WPARAM) hwnd,		//  wParam，源窗口句柄
					(LPARAM) &cds			//  lParam，COPYDATASTRUCT
					); 
				// 释放内存					
				HeapFree(GetProcessHeap(),HEAP_NO_SERIALIZE,szText);
			} 
			break; 
		}
	case WM_DESTROY:	// 退出
		PostQuitMessage(0); 
		return 0; 

	default:		// 其他消息的处理
		return DefWindowProc(hwnd, message, wParam, lParam); 
	}
return 0; 
}