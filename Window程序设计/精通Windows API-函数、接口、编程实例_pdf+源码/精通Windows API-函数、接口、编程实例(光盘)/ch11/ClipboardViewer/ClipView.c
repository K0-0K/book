/* ************************************
*《精通Windows API》
* 示例代码
* ClipView.c
* 11.3.2 监视粘贴板
**************************************/
/* 头文件 */
#include <windows.h>
#include <commctrl.h>
#include "resource.h"
/* 全局变量 */
HINSTANCE hinst;
UINT uFormat = (UINT)(-1);
BOOL fAuto = TRUE;
HWND hwnd;

LPSTR szFormatText = "CF_TEXT";
LPSTR szFormatOwnerdiplay = "CF_OWNERDISPLAY";
LPSTR szFormatEnhmetafile = "CF_ENHMETAFILE";
LPSTR szFormatBitMap = "CF_BITMAP";
/* 函数声明 */
void WINAPI InitMenu(HWND hwnd, HMENU hmenu) ;
BOOL WINAPI IsDisplayableFormat(UINT uFormat) ;
void WINAPI SetAutoView(HWND hwnd) ;
BOOL InitApplication(HINSTANCE hinstance);
BOOL InitInstance(HINSTANCE hinstance, int nCmdShow);
LPSTR GetPredefinedClipboardFormatName(UINT);
LRESULT CALLBACK MainWndProc( HWND ,UINT , WPARAM , LPARAM );

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
	// 消息循环
	while ((fGotMessage = GetMessage(&msg, (HWND) NULL, 0, 0)) != 0 && fGotMessage != -1)
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
BOOL InitApplication(HINSTANCE hinstance)
{
	// 使用RegisterClassEx
	WNDCLASSEX wcx;
	// 填充结构
	wcx.cbSize = sizeof(wcx); // WNDCLASSEX结构的大小
	wcx.style = CS_HREDRAW | CS_VREDRAW; // 如果大小改变了重绘窗口
	wcx.lpfnWndProc = MainWndProc; // 窗口消息处理函数
	wcx.cbClsExtra = 0; // 无附加窗口类内存
	wcx.cbWndExtra = 0; // 无附加窗口内存
	wcx.hInstance = hinstance; // 应用程序实例
	wcx.hIcon = NULL; // 图标
	wcx.hCursor = LoadCursor(NULL, IDC_ARROW); // 鼠标指针
	wcx.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); // 背景画刷
	wcx.lpszMenuName = NULL;//MAKEINTRESOURCE(IDR_MENU_MAIN); // 菜单资源
	wcx.lpszClassName = "MainWClass"; // 窗口类名
	wcx.hIconSm = NULL;
	// 注册窗口类，返回
	return RegisterClassEx(&wcx);
}

/* ************************************
* BOOL InitInstance(HINSTANCE hinstance, int nCmdShow)
* 功能 创建主窗口和控件
* 参数 hinstance，应用程序本次运行实例
* nCmdShow，如何显示
* 返回值 是否成功
**************************************/
BOOL InitInstance(HINSTANCE hinstance, int nCmdShow)
{
	// 保存应用程序实例句柄
	hinst = hinstance;
	hwnd = CreateWindow(
		"MainWClass", // 窗口类名，使用之前注册的主窗口类
		"ClipBoardViewer", // 窗口名，显示在窗口标题栏上的字符串
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS |
		WS_POPUPWINDOW, // 窗口样式
		CW_USEDEFAULT, // 水平位置（默认）
		CW_USEDEFAULT, // 垂直位置（默认）
		800, // 宽
		600, // 高
		(HWND) NULL, // 无父窗口
		(HMENU) LoadMenu(hinst,MAKEINTRESOURCE(IDR_MENU_COMMAND)), // 菜单
		hinstance, //应用程序实例
		(LPVOID) NULL);// 无窗口创建数据
	// 窗口是否创建成功
	if (!hwnd)
		return FALSE;
	// 显示、刷新窗口
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);
	return TRUE;
}
/* ************************************
* MainWndProc
* 窗口消息处理函数
**************************************/
LRESULT CALLBACK MainWndProc(
							 HWND hwnd,
							 UINT uMsg,
							 WPARAM wParam,
							 LPARAM lParam)
{
	static HWND hwndNextViewer;

	HDC hdc;
	HDC hdcMem;
	PAINTSTRUCT ps;
	LPPAINTSTRUCT lpps;
	RECT rc;
	LPRECT lprc;
	HGLOBAL hglb;
	LPSTR lpstr;
	HBITMAP hbm;
	HENHMETAFILE hemf;
	HWND hwndOwner;

	switch (uMsg)
	{
	case WM_PAINT: // 收到WM_PAINT消息后显示粘贴板中的数据
		hdc = BeginPaint(hwnd, &ps);
		// uForamt全局变量，在WM_DRAWCLIPBOARD和WM_COMMAND
		// 消息处理中，使用SetAutoView设置
		// 根据粘贴板中数据的不同格式，使用不同的显示方式
		switch (uFormat)
		{
			// Owner-display 格式，粘贴板的所有者必须显示并刷新Viewer的窗口
		case CF_OWNERDISPLAY:
			// 获取粘贴板的所有者
			hwndOwner = GetClipboardOwner();
			// 获取粘贴板数据
			hglb = GlobalAlloc(GMEM_MOVEABLE, sizeof(PAINTSTRUCT));
			lpps = GlobalLock(hglb);
			memcpy(lpps, &ps, sizeof(PAINTSTRUCT));
			GlobalUnlock(hglb);
			// 向粘贴板所有都发送WM_PAINTCLIPBOARD消息
			SendMessage(hwndOwner, WM_PAINTCLIPBOARD,
				(WPARAM) hwnd, (LPARAM) hglb);
			GlobalFree(hglb);
			break;
			// 位图
		case CF_BITMAP:
			// 创建DC
			hdcMem = CreateCompatibleDC(hdc);
			if (hdcMem != NULL)
			{
				if (OpenClipboard(hwnd)) // 打开粘贴板
				{
					// 获取粘贴板数据
					hbm = (HBITMAP) GetClipboardData(uFormat);
					// 将位图选择入DC，显示在窗口客户区
					SelectObject(hdcMem, hbm);
					GetClientRect(hwnd, &rc);
					BitBlt(hdc, 0, 0, rc.right, rc.bottom,
						hdcMem, 0, 0, SRCCOPY);
					// 关闭粘贴板
					CloseClipboard();
				}
				// 释放DC
				DeleteDC(hdcMem);
			}
			break;
			// 文本
		case CF_TEXT:
			if (OpenClipboard(hwnd)) // 打开粘贴板
			{
				// 获得粘贴板数据
				hglb = GetClipboardData(uFormat);
				lpstr = GlobalLock(hglb);
				// 将文件绘制在窗口客户区
				GetClientRect(hwnd, &rc);
				DrawText(hdc, lpstr, -1, &rc, DT_LEFT);

				GlobalUnlock(hglb);
				// 关闭粘贴板
				CloseClipboard();
			}
			break;
			// enhanced-format metafile，加强格式图元文件
		case CF_ENHMETAFILE:
			// 打开粘贴板
			if (OpenClipboard(hwnd))
			{
				// 获取数据
				hemf = GetClipboardData(uFormat);
				// 调用PlayEnhMetaFile在窗口客户区上显示
				GetClientRect(hwnd, &rc);
				PlayEnhMetaFile(hdc, hemf, &rc);
				CloseClipboard();
			}
			break;

		case 0: // 粘贴板为空
			GetClientRect(hwnd, &rc);
			// 在窗户区中央显示
			DrawText(hdc, "The clipboard is empty.", -1,
				&rc, DT_CENTER | DT_SINGLELINE |
				DT_VCENTER);
			break;

		default: // 不支持其他格式
				//CF_DIB
				//CF_DIBV5
				//CF_DIF
				//CF_DSPBITMAP
				//CF_DSPENHMETAFILE
				//CF_DSPMETAFILEPICT
				//CF_DSPTEXT
				//CF_ENHMETAFILE
				//CF_HDROP
				//CF_LOCALE
				//CF_METAFILEPICT
				//CF_PALETTE
				//CF_PENDATA
				//CF_PRIVATEFIRST
				//CF_RIFF
				//CF_SYLK
				//CF_WAVE
				//CF_TIFF
				//CF_UNICODETEXT
				GetClientRect(hwnd, &rc);
			DrawText(hdc, "Unable to display format.", -1,
				&rc, DT_CENTER | DT_SINGLELINE |
				DT_VCENTER);
		}
		EndPaint(hwnd, &ps);
		break;

	case WM_SIZE: // 如果窗口大小改变，通知粘贴板所有者窗口
		if (uFormat == CF_OWNERDISPLAY)
		{
			hwndOwner = GetClipboardOwner();
			hglb = GlobalAlloc(GMEM_MOVEABLE, sizeof(RECT));
			lprc = GlobalLock(hglb);
			GetClientRect(hwnd, lprc);
			GlobalUnlock(hglb);

			SendMessage(hwndOwner, WM_SIZECLIPBOARD,
				(WPARAM) hwnd, (LPARAM) hglb);

			GlobalFree(hglb);
		}
		break;

	case WM_CREATE:

		// 在窗口创建时，在粘贴板Viewer链中增加一个Viewer
		hwndNextViewer = SetClipboardViewer(hwnd);
		break;

	case WM_CHANGECBCHAIN://Viewer链中的节点变化时会收到此消息

		// 在链中增加本Viewer后，位于链的第一个节点
		// 当链中有Viewer移除时，链中的第一个Viewer会收到此消息
		// 如果是本Viewer的下一个Viewer移除，那么更新
		if ((HWND) wParam == hwndNextViewer)
			hwndNextViewer = (HWND) lParam;
		// 否则将消息向下一个Viewer传递
		else if (hwndNextViewer != NULL)
			SendMessage(hwndNextViewer, uMsg, wParam, lParam);
		break;

	case WM_DESTROY:// 窗口关闭
		// 设置Viewer链，移除本Viewer。
		ChangeClipboardChain(hwnd, hwndNextViewer);
		// 退出
		PostQuitMessage(0);
		break;

	case WM_DRAWCLIPBOARD: // 粘贴板的内容改变时会收到此消息

		// 设置显示格式为Auto
		SetAutoView(hwnd);

		// 将消息发送给Viewer链中的下一个窗口
		SendMessage(hwndNextViewer, uMsg, wParam, lParam);
		break;

	case WM_INITMENUPOPUP:// 当POPUP菜单弹出时收到此消息
		// 根据粘贴板中内容的格式设置菜单
		if (!HIWORD(lParam))
			InitMenu(hwnd, (HMENU) wParam);
		break;

	case WM_COMMAND: // 处理用户菜单输入
		switch (LOWORD(wParam))
		{
		case ID_NULL:
			break;

		case ID_AUTO: // 点击auto菜单项
			SetAutoView(hwnd); // 设置显示格式为自动
			break;

		default:
			// 用户选择了特定的显示格式
			// 设置显示模式，uFormat全局变量
			// uFormat在WM_PAINT消息时引用
			fAuto = FALSE;
			uFormat = LOWORD(wParam);
			InvalidateRect(hwnd, NULL, TRUE);
		}
		break;

	default: // 其他消息
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return (LRESULT) NULL;
}

/* ************************************
* void WINAPI SetAutoView(HWND hwnd)
* 获取粘贴板的主要格式，并设置显示方式
**************************************/
void WINAPI SetAutoView(HWND hwnd)
{
	static UINT auPriorityList[] = {
		CF_OWNERDISPLAY,
		CF_TEXT,
		CF_ENHMETAFILE,
		CF_BITMAP
	};
	// 获取粘贴板主要格式
	// 设置显示模式
	// uFormat在WM_PAINT消息时引用
	uFormat = GetPriorityClipboardFormat(auPriorityList, 4);
	fAuto = TRUE;

	InvalidateRect(hwnd, NULL, TRUE);
	UpdateWindow(hwnd);
}

/* ************************************
* void WINAPI InitMenu(HWND hwnd, HMENU hmenu)
* 功能 根据粘贴板中内容的格式，设置菜单项供用户选择显示方式
* 参数 hwnd，窗口句柄
* hmenu，需要设置的菜单句柄
**************************************/
void WINAPI InitMenu(HWND hwnd, HMENU hmenu)
{
	UINT uFormat;
	char szFormatName[80];
	LPCSTR lpFormatName;
	UINT fuFlags;
	UINT idMenuItem;

	// 判断菜单的第一项是不是auto
	// 所有的显示根据附加到这个POPUP中
	if (GetMenuItemID(hmenu, 0) != ID_AUTO)
		return;

	// 将除了第一个以外的其他所有菜单项删除
	while (GetMenuItemCount(hmenu) > 1)
		DeleteMenu(hmenu, 1, MF_BYPOSITION);

	// Auto项是否设置
	fuFlags = fAuto ? MF_BYCOMMAND | MF_CHECKED :
		MF_BYCOMMAND | MF_UNCHECKED;
	CheckMenuItem(hmenu, ID_AUTO, fuFlags);

	// 检测粘贴板中格式的数量
	if (CountClipboardFormats() == 0)
		return;

	// 打开粘贴板
	if (!OpenClipboard(hwnd))
		return;

	// 为每个格式附加一个菜单项
	AppendMenu(hmenu, MF_SEPARATOR, 0, NULL);
	uFormat = EnumClipboardFormats(0);

	while (uFormat)
	{
		// 程序为每一个格式定义了一个在菜单项中显示的名字
		lpFormatName = GetPredefinedClipboardFormatName(uFormat);

		// 如果程序未定义，获取格式的名字
		if (lpFormatName == NULL)
		{
			// 注意溢出
			if (GetClipboardFormatName(uFormat, szFormatName,
				sizeof(szFormatName)))
				lpFormatName = szFormatName;
			else
				lpFormatName = "(unknown)";
		}

		// 是可显示的格式
		if (IsDisplayableFormat(uFormat))
		{
			fuFlags = MF_STRING;
			idMenuItem = uFormat;
		}
		else
		{
			fuFlags = MF_STRING | MF_GRAYED;
			idMenuItem = 0;
		}
		// 增加菜单项
		AppendMenu(hmenu, fuFlags, idMenuItem, lpFormatName);
		// 下一个格式，循环
		uFormat = EnumClipboardFormats(uFormat);
	}
	CloseClipboard();

}

/* ************************************
* LPSTR GetPredefinedClipboardFormatName(UINT uFormat)
* 功能 根据格式返回格式名字符串，用于在菜单上显示
* 参数 uFormat，格式
* 返回值 LPSTR，格式名字符串，注意不能返回局部变量
**************************************/
LPSTR GetPredefinedClipboardFormatName(UINT uFormat)
{
	switch (uFormat)
	{
	case CF_OWNERDISPLAY:
		return szFormatOwnerdiplay;
	case CF_TEXT:
		return szFormatText;
	case CF_ENHMETAFILE:
		return szFormatEnhmetafile;
	case CF_BITMAP:
		return szFormatBitMap;
	default:
		return NULL;
	}
}

/* ************************************
* BOOL WINAPI IsDisplayableFormat(UINT uFormat)
* 功能 判断是否可显示的格式
* 参数 uFormat，格式
* 返回值 是，否
**************************************/
BOOL WINAPI IsDisplayableFormat(UINT uFormat)
{
	switch (uFormat)
	{
	case CF_OWNERDISPLAY:
	case CF_TEXT:
	case CF_ENHMETAFILE:
	case CF_BITMAP:
		return TRUE;
	}
	return FALSE;
}