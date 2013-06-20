/* ************************************
*《精通Windows API》
* 示例代码
* CopyRecv.c
* 11.4 通过WM_COPYDATA进程间通信
**************************************/
/* 头文件 */
#include <windows.h>
#include <commctrl.h>
/* 全局变量 */
HINSTANCE hinst;
HWND hwnd,hwndEdit;
/* 函数声明 */
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int);
LRESULT CALLBACK MainWndProc(HWND, UINT, WPARAM, LPARAM);

/* ************************************
* void main()
* 功能 进程间mailslot通信客户端
**************************************/
int WINAPI WinMain(
				   HINSTANCE hinstance,
				   HINSTANCE hPrevInstance,
				   LPSTR lpCmdLine,
				   int nCmdShow)
{
	MSG msg;
	WNDCLASSEX wcx;
	// 填充WNDCLASSEX
	wcx.cbSize = sizeof(wcx);
	wcx.style = CS_HREDRAW | CS_VREDRAW;
	wcx.lpfnWndProc = MainWndProc;
	wcx.cbClsExtra = 0;
	wcx.cbWndExtra = 0;
	wcx.hInstance = hinstance;
	wcx.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcx.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcx.hbrBackground = (HBRUSH)GetStockObject( WHITE_BRUSH);
	wcx.lpszMenuName = NULL;
	wcx.lpszClassName = "copydata2";
	wcx.hIconSm = NULL;
	// 注册窗口类
	if(RegisterClassEx(&wcx) == 0)
	{
		return 0;
	}
	// 创建窗口
	hwnd = CreateWindow(
		"copydata2",
		"RECV COPY DATA",
		WS_TILEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 500, 400,
		(HWND) NULL, (HMENU) NULL, hinstance, (LPVOID) NULL
		);

	if (!hwnd)
		return 0;
	// 显示刷新窗口
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);
	// 消息
	BOOL fGotMessage;
	while ((fGotMessage = GetMessage(&msg, (HWND) NULL, 0, 0)) != 0 && fGotMessage != -1)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
	UNREFERENCED_PARAMETER(lpCmdLine);
	return 0;
}

// 消息处理函数
LONG APIENTRY MainWndProc(HWND hwnd,
						  UINT message,
						  WPARAM wParam,
						  LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		{
			// 创建主窗口时，创建一个EDIT控件
			RECT rectMain;
			GetClientRect(hwnd,&rectMain);
			hwndEdit = CreateWindow("EDIT",
				NULL,
				WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_BORDER |
				ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL,
				0., 0, rectMain.right, rectMain.bottom,
				hwnd,
				NULL,
				(HINSTANCE) GetWindowLong(hwnd, GWL_HINSTANCE),
				NULL);
			if(hwndEdit == NULL)
			{
				MessageBox(hwnd, "Create Window Error","ERROR",MB_OK);
				ExitProcess(0);
			}
			break;
	case WM_COPYDATA:
		{
			// 当收到WM_COPYDATA消息时，从lParam中提取出
			// COPYDATASTRUCT
			PCOPYDATASTRUCT lpcds = (PCOPYDATASTRUCT)lParam;
			// 将复制的数据在界面上显示
			SetWindowText(hwndEdit,(LPSTR)lpcds->lpData);
			break;
		}

	case WM_DESTROY: // 退出
		PostQuitMessage(0);
		return 0;

	default: // 其他消息处理
		break;
		}
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}