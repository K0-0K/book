/* ************************************
*《精通Windows API》
* 示例代码
* text.c
* 13.4 文本和字体
**************************************/
/* 头文件 */
#include <Windows.h>
/*************************************
* HFONT ChooseNewFont()  
* 功能 选择字体
*
* 返回值 返回字体句柄
**************************************/
HFONT ChooseNewFont() 
{ 
    CHOOSEFONT cf; 
    LOGFONT lf; 
    HFONT hfont; 
 
    // CHOOSEFONT 结构 
    cf.lStructSize = sizeof(CHOOSEFONT); 
    cf.hwndOwner = (HWND)NULL; 
    cf.hDC = (HDC)NULL; 
    cf.lpLogFont = &lf; 
    cf.iPointSize = 0; 
    cf.Flags = CF_SCREENFONTS; 
    cf.rgbColors = RGB(0,0,0); 
    cf.lCustData = 0L; 
    cf.lpfnHook = (LPCFHOOKPROC)NULL; 
    cf.lpTemplateName = (LPSTR)NULL; 
    cf.hInstance = (HINSTANCE) NULL; 
    cf.lpszStyle = (LPSTR)NULL; 
    cf.nFontType = SCREEN_FONTTYPE; 
    cf.nSizeMin = 0; 
    cf.nSizeMax = 0; 
 
    // 选择字体对话框 
    ChooseFont(&cf); 
	// 得到HFONT 返回
    hfont = CreateFontIndirect(cf.lpLogFont); 
    return (hfont); 
} 
/*************************************
* WinMain  
* 功能 选择字体，并将文字显示在界面上
*
**************************************/
int WINAPI WinMain(
			HINSTANCE hInstance,
			HINSTANCE hPrevInstance,
			LPSTR lpCmdLine,
			int nCmdShow
			)
{
	HDC hdc = GetDC(NULL);

	int XIncrement = 10;
	int YStart = 50;
	TEXTMETRIC tm; 
	HFONT hfntDefault, hfntChoose; 
	SIZE sz; 
	UINT uAlignPrev;
	LPSTR lpszString1 = "字符串一"; 
	LPSTR lpszString2 = "字符串二"; 
	LPSTR lpszString3 = "字符串三";  
	DWORD dwStrLen1 = lstrlen(lpszString1);
	DWORD dwStrLen2 = lstrlen(lpszString2);
	DWORD dwStrLen3 = lstrlen(lpszString3);

	// 选择字体
	hfntChoose = ChooseNewFont(); 
	// 设置颜色
	SetBkColor(hdc,RGB(255,255,255));
	SetTextColor(hdc,RGB(255,0,0));
	SetBkMode(hdc,TRANSPARENT);

	// 输出字符串一
	TextOut(hdc, XIncrement, YStart, lpszString1, dwStrLen1); 

	// 为字符串二设置输出位置
	GetTextExtentPoint32(hdc, lpszString1, dwStrLen1, &sz); 
	XIncrement += sz.cx; 
	GetTextMetrics(hdc, &tm); 
	XIncrement -= tm.tmOverhang; 
	// 改变字体
	hfntDefault = SelectObject(hdc, hfntChoose); 	
	// 输出字符串二
	TextOut(hdc, XIncrement, YStart, lpszString2, dwStrLen2); 
	// 设置字符串三的输出位置
	GetTextExtentPoint32(hdc, lpszString1, dwStrLen1, &sz); 
	XIncrement = 10; 
	YStart += sz.cy;
	GetTextMetrics(hdc, &tm); 
	XIncrement -= tm.tmOverhang;
	// 设置为默认字体
	SelectObject(hdc, hfntDefault);
	// 输出字符串三
	uAlignPrev = SetTextAlign(hdc, TA_UPDATECP);    
	MoveToEx(hdc, XIncrement, YStart, (LPPOINT)NULL);
	TextOut(hdc, 0, 0, lpszString3, dwStrLen3); 
	SetTextAlign(hdc, uAlignPrev); 
	// Clear
	DeleteObject(hfntChoose); 
	SetBkMode(hdc, OPAQUE); 
	DeleteDC( hdc );
	return 0;
}