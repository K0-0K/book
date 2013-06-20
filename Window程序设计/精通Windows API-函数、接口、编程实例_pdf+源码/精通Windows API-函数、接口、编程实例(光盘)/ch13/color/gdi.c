/* ************************************
*《精通Windows API》
* 示例代码
* gdi.c
* 13.3  最简单的GDI程序
**************************************/
/* 头文件 */
#include <Windows.h>
/* 函数声明 */
void GdiOut(HDC hdc);

// WinMain
int WINAPI WinMain(
				   HINSTANCE hInstance,
				   HINSTANCE hPrevInstance,
				   LPSTR lpCmdLine,
				   int nCmdShow
				   )
{
	HDC hdc = GetDC(NULL);
	GdiOut(hdc);
	ReleaseDC(NULL, hdc);
}
/*************************************
* VOID GdiOut(HDC hdc) 
* 功能 演示GDI基本功能的使用
* 参数 HDC hdc，用于显示所绘制的图像
* 无返回值
**************************************/
VOID GdiOut(HDC hdc) 
{
	HPEN hpen, hpenOld;
	HBRUSH hbrush, hbrushOld;
	// 初始的颜色
	BYTE bRed  = 0;
	BYTE bGreen  = 0;
	BYTE bBlue = 0;
	// 笔的颜色 正黑
	COLORREF cPen = RGB(bRed, bGreen, bBlue);
	// 从COLORREF类型拆解颜色，设置笔刷的颜色，这里为紫偏蓝
	COLORREF cBrush = RGB(233, GetGValue(cPen),255);
	// 创建笔
	hpen = CreatePen(PS_SOLID, 10, cPen);
	// 创建笔刷
	hbrush = CreateSolidBrush(cBrush);
	// 为DC选择笔和笔刷
	hpenOld = SelectObject(hdc, hpen);
	hbrushOld = SelectObject(hdc, hbrush);
	// 绘制线条
	LineTo(hdc,500,500);
	// 使用初始的笔
	SelectObject(hdc,hpenOld);
	// 绘制矩形
	Rectangle( hdc, 200, 200, 500, 500 );
	// 释放资源
	DeleteObject(hpen);
	SelectObject(hdc, hbrushOld);
	DeleteObject(hbrush);
}