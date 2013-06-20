#include <Windows.h>
#include <math.h>
#include "resource.h"

CHOOSEFONT cf;          // common dialog box font structure 
LOGFONT lf;             // logical font structure 
HFONT hfont;            // new logical font handle 
HFONT hfontOld;         // original logical font handle 
HDC hdc;                // display DC handle 
int nXStart, nYStart;   // drawing coordinates 
RECT rc;                // structure for painting window 
SIZE sz;                // structure that receives text extents 
double aflSin[90];      // sine of 0-90 degrees 
double aflCos[90];      // cosine of 0-90 degrees 
double flRadius,a;      // radius of circle 
int iMode;              // clipping mode 
HRGN hrgn;              // clip region handle 
int i;
COLORREF  crOld;

LRESULT APIENTRY MainWndProc( 
							 HWND hwnd,                // window handle 
							 UINT message,             // type of message 
							 WPARAM wParam,            // additional information 
							 LPARAM lParam)            // additional information 

{ 

	PAINTSTRUCT ps; 

	switch (message) 
	{ 
	case WM_PAINT: 
		hdc = BeginPaint(hwnd, &ps); 
		EndPaint(hwnd, &ps); 
		break; 

	case WM_COMMAND:     // command from app's menu 
		switch (wParam) 
		{ 
		case IDM_VANISH:  // erases client area 
			hdc = GetDC(hwnd); 
			GetClientRect(hwnd, &rc); 
			FillRect(hdc, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH)); 
			ReleaseDC(hwnd, hdc); 
			break; 

		case IDM_AND: // sets clip mode to RGN_AND 
			iMode = RGN_AND; 
			break; 

		case IDM_COPY: // sets clip mode to RGN_COPY 
			iMode = RGN_COPY; 
			break; 

		case IDM_DIFF: // sets clip mode to RGN_DIFF 
			iMode = RGN_DIFF; 
			break; 

		case IDM_OR: // sets clip mode to RGN_OR 
			iMode = RGN_OR; 
			break; 

		case IDM_XOR: // sets clip mode to RGN_XOR 
			iMode = RGN_XOR; 
			break; 

		case IDM_CLIP_PATH: 
			{

				// Retrieve a cached DC for the window. 

				hdc = GetDC(hwnd); 

				// Use the font requested by the user in the 
				// Choose Font dialog box to create a logical
				// font, then select that font into the DC. 

				hfont = CreateFontIndirect(cf.lpLogFont); 
				hfontOld = (HFONT)SelectObject(hdc, hfont); 


				POINT point[5]= {{0,200},{600,200},{100,600},{300,0},{500,600}};

				hrgn = CreatePolygonRgn(point, 5, WINDING);



				SelectClipRgn(hdc, hrgn); 


				LPSTR szOut = "Lorem ipsum dolor sit amet, consectetur \n"
					"adipisicing elit, sed do eiusmod tempor \n" 
					"incididunt ut labore et dolore magna \n" 
					"aliqua. Ut enim ad minim veniam, quis \n" 
					"nostrud exercitation ullamco laboris nisi \n" 
					"ut aliquip ex ea commodo consequat. Duis \n" 
					"aute irure dolor in reprehenderit in \n" 
					"voluptate velit esse cillum dolore eu \n" 
					"fugiat nulla pariatur. Excepteur sint \n" 
					"occaecat cupidatat non proident, sunt \n" 
					"in culpa qui officia deserunt mollit \n" 
					"anim id est laborum.\n"; 
				RECT rect;
				GetClientRect(hwnd,&rect);
				DrawText(hdc, szOut, lstrlen(szOut),&rect , DT_CENTER);
				BeginPath(hdc); 


				Ellipse(hdc,100,100,500,500);

				//TextOut(hdc, nXStart, nYStart, szOut, lstrlen(szOut)); 
				EndPath(hdc); 
				SelectClipPath(hdc, iMode); 

				// Compute the sine of 0, 1, ... 90 degrees. 
				for (i = 0; i < 90; i++) 
				{ 
					aflSin[i] = sin( (((double)i) / 180.0) 
						* 3.14159); 
				} 

				// Compute the cosine of 0, 1,... 90 degrees. 
				for (i = 0; i < 90; i++) 
				{ 
					aflCos[i] = cos( (((double)i) / 180.0) 
						* 3.14159); 
				} 

				// Set the radius value. 

				flRadius = 1000; 

				// Draw the 90 rays extending from the 
				// radius to the edge of the circle. 

				for (i = 0; i < 90; i++) 
				{ 
					MoveToEx(hdc, nXStart, nYStart, 
						(LPPOINT) NULL); 
					LineTo(hdc, nXStart + ((int) (flRadius 
						* aflCos[i])), 
						nYStart + ((int) (flRadius 
						* aflSin[i]))); 
				} 

				// Reselect the original font into the DC. 

				SelectObject(hdc, hfontOld); 

				// Delete the user's font. 

				DeleteObject(hfont); 

				// Release the DC. 

				ReleaseDC(hwnd, hdc); 

				UpdateWindow(hwnd);

				break; 
			}


		case IDM_FONT: 

			// Initialize necessary members. 

			cf.lStructSize = sizeof (CHOOSEFONT); 
			cf.hwndOwner = hwnd; 
			cf.lpLogFont = &lf; 
			cf.Flags = CF_SCREENFONTS | CF_EFFECTS; 
			cf.rgbColors = RGB(0, 255, 255); 
			cf.nFontType = SCREEN_FONTTYPE; 

			// Display the Font dialog box, allow the user 
			// to choose a font, and render text in the 
			// window with that selection. 

			if (ChooseFont(&cf)) 
			{ 
				hdc = GetDC(hwnd); 
				hfont = CreateFontIndirect(cf.lpLogFont); 
				hfontOld = (HFONT)SelectObject(hdc, hfont); 
				crOld = SetTextColor(hdc, cf.rgbColors); 
				//TextOut(hdc, nXStart, nYStart, 
				//    "Clip Path", 9); 
				SetTextColor(hdc, crOld); 
				SelectObject(hdc, hfontOld); 
				DeleteObject(hfont); 
				ReleaseDC(hwnd, hdc); 
			} 

			break; 

		default: 
			return DefWindowProc(hwnd, message, wParam, 
				lParam); 
		} 
		break; 

	case WM_DESTROY:    // window is being destroyed 
		PostQuitMessage(0); 
		break; 

	default: 
		return DefWindowProc(hwnd, message, wParam, lParam); 
	} 
	return 0; 
} 


int WINAPI WinMain(
				   HINSTANCE hInstance,
				   HINSTANCE hPrevInstance,
				   LPSTR lpCmdLine,
				   int nCmdShow)
{
	MSG msg;
	HWND hWnd;
	WNDCLASS wc;


	wc.style = CS_OWNDC; 
	wc.lpfnWndProc = (WNDPROC)MainWndProc; 
	wc.cbClsExtra = 0; 
	wc.cbWndExtra = 0; 
	wc.hInstance = hInstance;
	wc.hIcon = NULL; 
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName = MAKEINTRESOURCE(IDR_MENU_CLIP);
	wc.lpszClassName = "shape"; 

	if (!RegisterClass(&wc))
	{
		return (FALSE);
	}

	hWnd = CreateWindow(
		"shape", 
		"shape", 
		WS_OVERLAPPEDWINDOW,
		100, 100, 600, 650, 
		NULL, 
		NULL,
		hInstance, 
		NULL 
		);

	if (!hWnd)
	{
		return (FALSE);
	}

	ShowWindow(hWnd, nCmdShow); 
	UpdateWindow(hWnd); 

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)(msg.wParam);

	UNREFERENCED_PARAMETER(lpCmdLine); 
}