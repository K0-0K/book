/* ************************************
*《精通Windows API》
* 示例代码
* ClipText.c
* 11.3.2 操作粘贴板
**************************************/
/* 头文件 */
#include "cliptext.h"
#include <string.h>
/* 预处理 */
#ifdef UNICODE// UNICODE
#define MY_CLIP_FORMAT CF_UNICODETEXT
#else
#define MY_CLIP_FORMAT CF_TEXT
#endif
/* 全局变量 */
HINSTANCE hInst;
HACCEL hAccTable;
HWND hwnd;
LPTSTR lpszText = NULL;
LPTSTR lpData = NULL; // 粘贴板数据指针
HANDLE hHeap;
LPTSTR szInitialClientAreaText =
TEXT("This program demonstrates the use of the Edit menu to copy and \n")
TEXT("paste text to and from the clipboard. Try using the Copy command \n")
TEXT("to move this text to the clipboard, and the Paste command to replace \n")
TEXT("this text with data from another application. \r\n\r\n")
TEXT("You might want to try running Notepad and Clipbrd alongside this \n")
TEXT("application so that you can watch the data exchanges take place. \n");

/* 函数声明 */
BOOL InitApplication(HANDLE);
BOOL InitInstance(HANDLE, INT);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);
VOID OutOfMemory(VOID);

/* ************************************
* WinMain
* 功能 程序入口点，调用InitApplication，InitInstance
* 消息循环
* 参数 hinstance，应用程序本次运行实例
* hPrevInstance，应用程序之前的实例，始终为NULL
* lpCmdLine，命令行参数
* nCmdShow，如果窗口，SW_SHOW
* 返回值 失败返回FALSE
**************************************/
int WINAPI WinMain(
				   HINSTANCE hinstance,
				   HINSTANCE hPrevInstance,
				   LPSTR lpCmdLine,
				   int nCmdShow)
{
	MSG msg;
	BOOL fGotMessage;
	// InitApplication
	if (!InitApplication(hinstance))
	{
		return FALSE;
	}
	// InitInstance
	if (!InitInstance(hinstance, nCmdShow))
	{
		return FALSE;
	}
	// 消息循环，只处理本窗口的消息
	while ((fGotMessage = GetMessage(&msg, (HWND) hwnd, 0, 0)) != 0 && fGotMessage != -1)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
	UNREFERENCED_PARAMETER(lpCmdLine);
}

/* ************************************
* BOOL InitApplication(HINSTANCE hinstance)
* 功能 注册主窗口类
* 参数 hinstance，应用程序本次运行实例
* 返回值 是否成功
**************************************/
BOOL InitApplication(
					 HANDLE hInstance)
{
	WNDCLASS wc;

	wc.lpfnWndProc = (WNDPROC) WndProc;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = TEXT("CliptextWClass");
	wc.style = 0 ;
	wc.cbClsExtra =0;
	wc.cbWndExtra =0;

	return (RegisterClass(&wc));
}

/* ************************************
* BOOL InitInstance(HINSTANCE hinstance, int nCmdShow)
* 功能 创建主窗口
* 参数 hinstance，应用程序本次运行实例
* nCmdShow，如何显示
* 返回值 是否成功
**************************************/
BOOL InitInstance(
				  HINSTANCE hInstance,
				  INT nCmdShow)
{
	// 保存实例
	hInst = hInstance;
	// 保存堆句柄，后面的程序分配内存
	hHeap = GetProcessHeap();
	// Accelerators
	hAccTable = LoadAccelerators(hInst, TEXT("ClipTextAcc"));
	// 为lpszText分配内存，lpszText是用于在界面上显示的文本
	lpszText = HeapAlloc(hHeap,0,lstrlen(szInitialClientAreaText)+1);
	// 复制初始显示的字符串
	lstrcpy(lpszText, szInitialClientAreaText);
	// 创建窗口
	hwnd = CreateWindow(
		TEXT("CliptextWClass"),
		TEXT("Cliptext Sample Application"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0,
		CW_USEDEFAULT, 0,
		NULL,
		(HMENU)LoadMenu(hInstance,MAKEINTRESOURCE(ID_CLIP_MENU)),
		hInstance,
		NULL );
	if (!hwnd)
	{
		return FALSE;
	}
	// 显示、刷新窗口
	ShowWindow(hwnd, nCmdShow);
	return UpdateWindow(hwnd);
}

/* ************************************
* WndProc
* 窗口消息处理
**************************************/
LRESULT CALLBACK WndProc(
						 HWND hWnd,
						 UINT message,
						 WPARAM wParam,
						 LPARAM lParam)
{
	HDC hDC = NULL;
	PAINTSTRUCT ps = {0};
	RECT rectClient = {0};
	LPTSTR lpClipData = NULL;

	switch (message)
	{
	case WM_INITMENU: // 初始化菜单
		if (wParam == (WPARAM)GetMenu(hWnd))
		{
			if (OpenClipboard(hWnd))
			{
				// 格式是否可用
				if (IsClipboardFormatAvailable(MY_CLIP_FORMAT) || 
					IsClipboardFormatAvailable(CF_OEMTEXT))
					// 菜单项
				{
					EnableMenuItem((HMENU)wParam, IDM_PASTE, MF_ENABLED);
				}
				else
				{
					EnableMenuItem((HMENU)wParam, IDM_PASTE, MF_GRAYED);
				}
				// 关闭粘贴板
				CloseClipboard();
				return (TRUE);
			}
			else /* Clipboard is not available */
			{
				return (FALSE);
			}
		}
		return (TRUE);

	case WM_COMMAND: // 菜单命令
		switch(LOWORD(wParam))
		{
		case IDM_ABOUT:
			break;
		case IDM_NEW:
		case IDM_OPEN:
		case IDM_SAVE:
		case IDM_SAVEAS:
		case IDM_PRINT:
		case IDM_UNDO:
		case IDM_CLEAR:
			MessageBox (GetFocus (),
				TEXT("Command not implemented."),
				TEXT("ClipText Sample Application"),
				MB_ICONASTERISK | MB_OK);
			// 以上不处理

		case IDM_EXIT: // 退出
			DestroyWindow(hWnd);
			break;

		case IDM_CUT:// 剪切和复制界面上显示的内容
		case IDM_COPY:
			if (lpszText != NULL) // 界面是否有显示的内容
			{
				if(lpData != NULL)
				{
					HeapFree(hHeap,0,lpData);
				}
				// 为粘贴板数据分配空间
				lpData = HeapAlloc(hHeap,0,HeapSize(hHeap,0,lpszText));
				// 复制
				lstrcpy(lpData, lpszText);

				if (OpenClipboard(hWnd)) // 打开粘贴板
				{
					// 设置粘贴板内容
					EmptyClipboard();
					SetClipboardData(MY_CLIP_FORMAT, lpData);
				}
				CloseClipboard();// 关闭

				if (LOWORD(wParam) == IDM_CUT)
				{
					//如果是剪切，清除屏幕内容，设置菜单
					HeapFree(hHeap,0,lpszText);
					lpszText = 0;
					EnableMenuItem(GetMenu (hWnd), IDM_CUT, MF_GRAYED);
					EnableMenuItem(GetMenu(hWnd), IDM_COPY, MF_GRAYED);
					InvalidateRect (hWnd, NULL, TRUE);
					UpdateWindow (hWnd);
				}
			}
			return (TRUE);

		case IDM_PASTE:
			if (OpenClipboard(hWnd))
			{
				// 从粘贴板获得文本
				if (!(lpClipData = GetClipboardData(MY_CLIP_FORMAT)))
				{
					CloseClipboard();
					break;
				}
				// 为新的显示文本分配空间
				if(lpszText != NULL)
				{
					HeapFree(hHeap,0,lpszText);
					lpszText = NULL;
				}
				lpszText = HeapAlloc(hHeap,0,lstrlen(lpClipData)+1);
				//复制
				lstrcpy(lpszText, lpClipData);
				CloseClipboard();// 关闭
				// 设置菜单项
				EnableMenuItem(GetMenu(hWnd), IDM_CUT, MF_ENABLED);
				EnableMenuItem(GetMenu(hWnd), IDM_COPY, MF_ENABLED);

				// 显示
				InvalidateRect(hWnd, NULL, TRUE);
				UpdateWindow(hWnd);
				return (TRUE);
			}
			else
			{
				return (FALSE);
			}
		}
		break;

	case WM_SIZE:
		InvalidateRect(hWnd, NULL, TRUE);
		break;

	case WM_PAINT:
		// 将文本显示在界面上
		hDC = BeginPaint (hWnd, &ps);

		if (lpszText != NULL)
		{
			GetClientRect (hWnd, &rectClient);
			DrawText (hDC, lpszText, -1, &rectClient,
				DT_EXTERNALLEADING | DT_NOPREFIX | DT_WORDBREAK);

		}
		EndPaint (hWnd, &ps);
		break;

	case WM_DESTROY:
		//释放内存，退出
		if (lpszText != NULL)
		{
			HeapFree(hHeap,0,lpszText);
		}
		if(lpData != NULL)
		{
			HeapFree(hHeap,0,lpData);
		}
		PostQuitMessage(0);
		break;

	default:
		return (DefWindowProc(hWnd, message, wParam, lParam));
	}
	return (0);
}