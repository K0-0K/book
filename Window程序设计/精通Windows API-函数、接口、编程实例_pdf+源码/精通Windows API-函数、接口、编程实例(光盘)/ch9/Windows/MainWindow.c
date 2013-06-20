/* ************************************
*《精通Windows API》 
* 示例代码
* MainWindow.c
* 9.3-9.6  图形用户界面
**************************************/
/* 头文件 */
#include <windows.h>
#include <commctrl.h>
#include <Winuser.h>
#include <shellapi.h>
#include "resource.h"
#include "Window.h"
/* 外部变量 */
extern HINSTANCE hinst;
extern HWND hwndMain;
extern HWND hwndTreeView;
extern HWND hwndListView;
extern HWND hwndEdit;
extern HMENU hMenuMain;
extern HMENU hPopup;
extern LPSTR lpszLatin; 

/* ************************************
* WinMain
* 功能	程序入口点，调用InitApplication，InitInstance
*			消息循环
* 参数	hinstance，应用程序本次运行实例
*			hPrevInstance，应用程序之前的实例，始终为NULL
*			lpCmdLine，命令行参数
*			nCmdShow，如果窗口，SW_SHOW
* 返回值	失败返回FALSE
**************************************/
int WINAPI WinMain(
				   HINSTANCE hinstance, HINSTANCE hPrevInstance,
				   LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	BOOL fGotMessage;
	//	 InitApplication
	if (!InitApplication(hinstance))
	{
		return FALSE;
	}
	//	 InitInstance
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
* 功能	注册主窗口类
* 参数	hinstance，应用程序本次运行实例
* 返回值	是否成功
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
	wcx.hIcon = LoadIcon(NULL,	MAKEINTRESOURCE(IDI_ICON_MAIN)); // 图标
	wcx.hCursor = LoadCursor(NULL,	IDC_ARROW); // 鼠标指针
	wcx.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); // 背景画刷
	wcx.lpszMenuName = MAKEINTRESOURCE(IDR_MENU_MAIN); // 菜单资源
	wcx.lpszClassName = "MainWClass"; // 窗口类名
	wcx.hIconSm = (HICON)LoadImage(hinstance, // 小图标
		MAKEINTRESOURCE(IDI_ICON_MAIN),
		IMAGE_ICON,
		GetSystemMetrics(SM_CXSMICON),
		GetSystemMetrics(SM_CYSMICON),
		LR_DEFAULTCOLOR);
	// 注册窗口类，返回
	return RegisterClassEx(&wcx);
}

/* ************************************
* BOOL InitInstance(HINSTANCE hinstance, int nCmdShow)
* 功能	创建主窗口和控件
* 参数	hinstance，应用程序本次运行实例
*			nCmdShow，如何显示
* 返回值	是否成功
**************************************/
BOOL InitInstance(HINSTANCE hinstance, int nCmdShow)
{
	HWND hwnd;
	RECT rect;
	// 保存应用程序实例句柄
	hinst = hinstance;
	hwnd = CreateWindow(
		"MainWClass", // 窗口类名，使用之前注册的主窗口类
		"Let's Smile", // 窗口名，显示在窗口标题栏上的字符串
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS |
		WS_POPUPWINDOW, 	// 窗口样式
		CW_USEDEFAULT, // 水平位置（默认）
		CW_USEDEFAULT, // 垂直位置（默认）
		800, // 宽
		600, // 高
		(HWND) NULL, // 无父窗口
		(HMENU) LoadMenu(hinst,MAKEINTRESOURCE(IDR_MENU_MAIN)), // 菜单
		hinstance, //应用程序实例
		(LPVOID) NULL); // 无窗口创建数据
	// 窗口是否创建成功
	if (!hwnd)
		return FALSE;
	// 保留窗口句柄
	hwndMain = hwnd;
	// 保证通用控件动态链接库已经加载
	InitCommonControls();
	// 创建三种控件，子窗口
	hwndTreeView = CreateTreeView(hwndMain, "files");
	hwndListView = CreateListView(hwndMain,"processing");
	hwndEdit = CreateEdit(hwndMain,"texta");
	// 获取本窗口客户区的RECT（矩形方框的四个边界点）
	GetClientRect(hwndMain,&rect);
	// 设置子窗口的大小的位置
	SetWindows(&rect);
	// 在EDIT控件中显示文本
	ShowTextOnEdit(lpszLatin);
	// 显示主窗口，使用WinMain函数设置的窗口显示方式
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);
	return TRUE;
}
/* ************************************
* MainWndProc
* 功能	主窗口消息处理函数
**************************************/
LRESULT CALLBACK MainWndProc(
							 HWND hwnd, // 窗口句柄
							 UINT uMsg, // 消息标识符
							 WPARAM wParam, // 消息的第一个参数
							 LPARAM lParam)// 消息的第二个参数
{

	switch (uMsg)
	{
	case WM_CREATE:	// 窗口创建时收到此消息
		break;

	case WM_PAINT:		// 窗口被绘制时收到此消息
		break;

	case WM_SIZE:		// 窗口大小改变时
		OnWindowResize();
		break;
		// NOTIFY，通常由控件发送给父窗口，说明控件正在进行某项窗口
	case WM_NOTIFY:
		OnChildWindowsNotify((PVOID)lParam);
		break;
		// 窗口销毁，点击右上角的关闭按钮会触发此消息
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
		// 命令输入，菜单项被选中，或按钮被点击
	case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
				// 在“帮助”菜单中选择“关于”
			case ID_HELP_ABOUT:
				{
					DialogBox(
						hinst, (LPCTSTR)IDD_DIALOG_ABOUT, hwnd, (DLGPROC)About
						);
					return 0;
				}
			default:
				return DefWindowProc(hwnd, uMsg, wParam, lParam);
			}
		}
		// 可以在此处理其他消息
	default:
		break;
	}
	// 有很多消息未做处理，需要由默认窗口消息处理函数来处理
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
/* ************************************
* BOOL OnWindowResize()
* 功能	处理窗口大小改变消息
*			改变子窗口大小和位置
**************************************/
BOOL OnWindowResize()
{
	RECT rcClient;
	GetClientRect(hwndMain, &rcClient);
	SetWindows(&rcClient);
	return TRUE;
}

/* ************************************
* BOOL OnChildWindowsNotify(PVOID pParam)
* 功能	处理控制子窗口向父窗口发送的NOTIFY消息
* 参数	pParam，WM_NOTIFY消息参数
* 返回值 	是否是Tree View发送的
**************************************/
BOOL OnChildWindowsNotify(PVOID pParam)
{
	LPNMHDR phdr = (LPNMHDR)pParam;
	// 只处理Tree View发送的Notify，其他不处理
	if(phdr->hwndFrom != hwndTreeView)
	{
		return FALSE;
	}
	switch (((LPNMHDR)pParam)->code)
	{
	// 如果是右键单击，则调用OnRclickTree函数弹出右键菜单
	case NM_RCLICK :
		OnRclickTree((LPNMHDR)pParam);
		break;

	default:
		break;
	}
	return TRUE;
}


/* ************************************
* About
* 功能	“关于”对话框消息处理函数
**************************************/
LRESULT CALLBACK About(
					   HWND hDlg,		// 对话框窗口句柄
					   UINT message,		// 消息
					   WPARAM wParam,	// 消息参数
					   LPARAM lParam)		// 消息参数
{
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
		// 点击OK，结束对话框
		if (LOWORD(wParam) == IDOK) 
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		// 点击“退出”，结束进程
		if(LOWORD(wParam) == IDCANCEL)
		{
			ExitProcess(0);
		}
		break;
	}
	return FALSE;
}

/* ************************************
* DWORD SetWindows(LPRECT lpRect)
* 功能	本程序中创建了三个控件，有三个子窗口
*			本函数设置子窗口的位置和大小			
* 参数	lpRect，指向表示父窗口客户区的RECT
**************************************/
DWORD SetWindows(LPRECT lpRect)
{
	// Tree View
	SetWindowPos(hwndTreeView, HWND_TOP,
		lpRect->left, lpRect->top,
		lpRect->right *0.3 ,lpRect->bottom,
		SWP_SHOWWINDOW);
	// List View
	SetWindowPos(hwndListView, HWND_TOP,
		lpRect->right * 0.3, lpRect->bottom * 0.7,
		lpRect->right * 0.7, lpRect->bottom * 0.3,
		SWP_SHOWWINDOW);
	// Edit
	SetWindowPos(hwndEdit, HWND_TOP,
		lpRect->right * 0.3, lpRect->top,
		lpRect->right * 0.7, lpRect->bottom * 0.7,
		SWP_SHOWWINDOW);
	return 0;
}