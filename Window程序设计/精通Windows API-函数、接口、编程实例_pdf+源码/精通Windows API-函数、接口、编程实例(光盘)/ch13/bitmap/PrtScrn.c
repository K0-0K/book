/* ************************************
*《精通Windows API》
* 示例代码
* PrtSrcn.c
* 13.7 截屏
**************************************/
/* 头文件 */
#include <windows.h>
/* 常量定义 */
#define PALVERSION 0x300
#define CAP_SHOW_MODE_STRTCH 1
#define CAP_SHOW_MODE_NOSTRTCH 0

/* 全局变量 */
HBITMAP ghBitmap = NULL;
RECT rectShow;
// 修改这里截取不同的窗口，如果为NULL，则截取屏幕
LPSTR szCaptureWindowName = "Windows 任务管理器";

/* 函数申明 */
LRESULT CALLBACK MainWndProc(HWND, UINT, WPARAM, LPARAM);
HBITMAP ScreenCapture(LPSTR filename ,WORD BitCount,LPRECT lpRect);
VOID DoPaint(HWND hWnd, DWORD dwMode);

/*************************************
* DWORD WINAPI WinMain(HINSTANCE hinstance,
HINSTANCE hPrevInstance,
LPSTR lpCmdLine,
int nCmdShow)
* 功能 截屏，保存为文件，并显示在窗口上
*
**************************************/
INT WINAPI WinMain(HINSTANCE hinstance,
					 HINSTANCE hPrevInstance,
					 LPSTR lpCmdLine,
					 int nCmdShow)
{
	WNDCLASSEX wcx;
	HWND hwnd;
	MSG msg;
	WORD wport = 80;
	BOOL fGotMessage;
	HWND hwndCap = NULL;

	// 截取全屏幕还是窗口
	if(szCaptureWindowName != NULL)
	{
		hwndCap = FindWindow(NULL,"Windows 任务管理器");
		// 获取窗口的RECT，可自行修改，获取屏幕中的任意区域
		if(!GetWindowRect(hwndCap,&rectShow))
		{
			MessageBox(NULL,"Can not find window to capture", "erroe",MB_OK);
			return 0;
		}
	}
	// 注册窗口类，并创建窗口，用于显示截取的位图
	wcx.cbSize = sizeof(wcx);
	wcx.style = CS_HREDRAW | CS_VREDRAW;
	wcx.lpfnWndProc = MainWndProc;
	wcx.cbClsExtra = 0;
	wcx.cbWndExtra = 0;
	wcx.hInstance = hinstance;
	wcx.hIcon = LoadIcon(NULL,	MAKEINTRESOURCE(IDI_APPLICATION));
	wcx.hCursor = LoadCursor(NULL,	IDC_ARROW);
	wcx.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wcx.lpszMenuName = NULL;
	wcx.lpszClassName = "MainWClass";
	wcx.hIconSm = NULL;

	if( !RegisterClassEx(&wcx))
		return 1;
	// 创建窗口
	hwnd = CreateWindow(
		"MainWClass",
		"CAP",
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS |
		WS_MAXIMIZE | WS_POPUPWINDOW,
		CW_USEDEFAULT,	CW_USEDEFAULT,	CW_USEDEFAULT,	CW_USEDEFAULT,
		(HWND) NULL,	(HMENU) NULL,	hinstance, (LPVOID) NULL);

	if (!hwnd)
		return 1;

	// 截取屏幕，可根据需要设置不同的参数，这里只演示截取特定窗口。
	ghBitmap = ScreenCapture("taskmgr.bmp" ,32, &rectShow);
	// 显示
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	while ((fGotMessage = GetMessage(&msg, (HWND) NULL, 0, 0)) != 0 && fGotMessage != -1)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
	UNREFERENCED_PARAMETER(lpCmdLine);
}

LRESULT CALLBACK MainWndProc(
							 HWND hwnd,
							 UINT uMsg,
							 WPARAM wParam,
							 LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_PAINT:
		// 显示截取的屏幕
		DoPaint(hwnd,CAP_SHOW_MODE_STRTCH);
		break;
	case WM_DESTROY:
		// 创建的BITMAP对象需要删除，以释放资源
		DeleteObject(ghBitmap);
		ExitProcess(0);
		break;
	default:
		break;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

/*************************************
* VOID DoPaint(HWND hWnd, DWORD dwMode)
* 功能 将位图（全局变量ghBitmap）显示在界面上
*
* 参数 HWND hWnd，用于显示位图的窗口
* DWORD dwMode，模式，是否拉申
*
* 无返回值
**************************************/
VOID DoPaint(HWND hWnd, DWORD dwMode)
{
	PAINTSTRUCT ps;
	RECT rect;
	HDC hdcMem;
	BITMAP bm;
	// BeginPaint
	HDC hDC = BeginPaint(hWnd, &ps);
	// 获取窗口的Client区域，用于显示位图
	GetClientRect(hWnd, &rect);

	// 设置拉申模式
	SetStretchBltMode(hDC, HALFTONE);
	// 将BITMAP对象选择入内存DC
	hdcMem = CreateCompatibleDC(hDC);
	SelectObject(hdcMem, ghBitmap);
	if (ghBitmap)
	{
		// 获取DIB属性
		if (GetObject(ghBitmap, sizeof(BITMAP), &bm))
		{
			// 判断参数示：是否根据显示窗口大小拉申位图
			// 采用不同的方面将内存DC StretchBl t至窗口Client区域DC
			if(dwMode == CAP_SHOW_MODE_STRTCH)
			{
				StretchBlt(hDC, 0, 0, rect.right, rect.bottom,
					hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
			}
			else
			{
				// 不拉伸，计算显示的位置，将其显示在Client的中央
				INT ixStart = (rect.right - rect.left - bm.bmWidth)/2;
				INT iyStart = (rect.bottom - rect.top - bm.bmHeight)/2;
				ixStart = ixStart < 0 ? 0 : ixStart;
				iyStart = iyStart < 0 ? 0 : iyStart;
				BitBlt(hDC, 0, 0, rect.right, rect.bottom,
					hdcMem,-ixStart,-iyStart, SRCCOPY);
			}
			DeleteDC(hdcMem);
		}
	}
	// 如果没有位图，则使用Brush填充
	else
	{
		PatBlt(hDC, 0, 0, rect.right, rect.bottom, BLACKNESS);
	}
	// EndPaint
	EndPaint(hWnd, &ps);
}

/*************************************
* BITMAP ScreenCapture(LPSTR filename ,WORD BitCount,LPRECT lpRect);
* 功能 截取指定区域的屏幕，并保存为文件
*
* 参数	LPSTR filename 保存位图文件的文件路径，如果为NULL，则不保存
*		WORD BitCount Bit深度，用于表示一个像素点所使用的数据长度
*		LPRECT lpRect 所需截取的屏幕区域，如果为NULL，则获取全屏幕
*
* 返回 HBITMAP 所截取的位图对象
**************************************/
HBITMAP ScreenCapture(LPSTR filename ,WORD BitCount,LPRECT lpRect)
{
	HBITMAP hBitmap;
	// 显示器屏幕DC
	HDC hScreenDC = CreateDC("DISPLAY", NULL, NULL, NULL);
	HDC hmemDC = CreateCompatibleDC(hScreenDC);
	// 显示器屏幕的宽和高
	int ScreenWidth = GetDeviceCaps(hScreenDC, HORZRES);
	int ScreenHeight = GetDeviceCaps(hScreenDC, VERTRES);
	// 旧的BITMAP，用于与所需截取的位置交换
	HBITMAP hOldBM;
	// 保存位图数据
	PVOID lpvpxldata;
	// 截屏获取的长宽及起点
	INT ixStart;
	INT iyStart;
	INT iX;
	INT iY;
	// 位图数据大小
	DWORD dwBitmapArraySize;
	// 几个大小
	DWORD nBitsOffset;
	DWORD lImageSize ;
	DWORD lFileSize ;
	// 位图信息头
	BITMAPINFO bmInfo;
	// 位图文件头
	BITMAPFILEHEADER bmFileHeader;
	// 写文件用
	HANDLE hbmfile;
	DWORD dwWritten;

	// 如果LPRECT 为NULL 截取整个屏幕
	if(lpRect == NULL)
	{
		ixStart = iyStart = 0;
		iX = ScreenWidth;
		iY =ScreenHeight;
	}
	else
	{
		ixStart = lpRect->left;
		iyStart = lpRect->top;
		iX = lpRect->right - lpRect->left;
		iY = lpRect->bottom - lpRect->top;
	}
	// 创建BTIMAP
	hBitmap = CreateCompatibleBitmap(hScreenDC, iX, iY);
	// 将BITMAP选择入内存DC。
	hOldBM = (HBITMAP)SelectObject(hmemDC, hBitmap);
	// BitBlt屏幕DC到内存DC，根据所需截取的获取设置参数
	BitBlt(hmemDC, 0, 0, iX, iY, hScreenDC, ixStart, iyStart, SRCCOPY);
	// 将旧的BITMAP对象选择回内存DC，返回值为被替换的对象，既所截取的位图
	hBitmap = (HBITMAP)SelectObject(hmemDC, hOldBM);
	if(filename == NULL)
	{
		DeleteDC( hScreenDC);
		DeleteDC(hmemDC);
		return hBitmap;
	}
	// 为位图数据申请内存空间
	dwBitmapArraySize = ((((iX*32) + 31) & ~31)>> 3)* iY;
	lpvpxldata = HeapAlloc(GetProcessHeap(),HEAP_NO_SERIALIZE,dwBitmapArraySize);
	ZeroMemory(lpvpxldata,dwBitmapArraySize);

	// 添充 BITMAPINFO 结构
	ZeroMemory(&bmInfo,sizeof(BITMAPINFO));
	bmInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmInfo.bmiHeader.biWidth = iX;
	bmInfo.bmiHeader.biHeight = iY;
	bmInfo.bmiHeader.biPlanes = 1;
	bmInfo.bmiHeader.biBitCount = BitCount;
	bmInfo.bmiHeader.biCompression = BI_RGB;

	// 添充 BITMAPFILEHEADER 结构
	ZeroMemory(&bmFileHeader,sizeof(BITMAPFILEHEADER));
	nBitsOffset = sizeof(BITMAPFILEHEADER) + bmInfo.bmiHeader.biSize;
	lImageSize =
		((((bmInfo.bmiHeader.biWidth * bmInfo.bmiHeader.biBitCount) + 31) & ~31)>> 3)
		* bmInfo.bmiHeader.biHeight;
	lFileSize = nBitsOffset + lImageSize;
	bmFileHeader.bfType = 'B'+('M'<<8);
	bmFileHeader.bfSize = lFileSize;
	bmFileHeader.bfOffBits = nBitsOffset;

	// 获取DIB用于写入到文件
	GetDIBits(hmemDC, hBitmap, 0, bmInfo.bmiHeader.biHeight,
		lpvpxldata, &bmInfo, DIB_RGB_COLORS);
	// 写文件
	hbmfile = CreateFile(filename,
		GENERIC_WRITE,
		FILE_SHARE_WRITE,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if(hbmfile == INVALID_HANDLE_VALUE)
	{
		MessageBox(NULL,"create file error","error",MB_OK);
	}

	WriteFile(hbmfile,&bmFileHeader,sizeof(BITMAPFILEHEADER),&dwWritten,NULL);
	WriteFile(hbmfile,&bmInfo,sizeof(BITMAPINFO),&dwWritten,NULL);
	WriteFile(hbmfile,lpvpxldata,lImageSize,&dwWritten,NULL);
	CloseHandle(hbmfile);

	// 释放内存，清除不同的DC。
	// 这里没有删除BITMAP对象，需在显示完成后删除
	HeapFree(GetProcessHeap(),HEAP_NO_SERIALIZE,lpvpxldata);
	ReleaseDC(0, hScreenDC);
	DeleteDC(hmemDC);	
	return hBitmap;
}