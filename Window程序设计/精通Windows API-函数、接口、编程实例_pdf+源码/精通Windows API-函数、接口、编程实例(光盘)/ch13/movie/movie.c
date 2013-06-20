#include <windows.h>


#define POLYCLASS  "POLYDEMO"

#define MAX_BEZIER 80
#define VELMAX     10
#define VELMIN      2
#define BEZ_CURVES  1              //Number of Curves in the Bezier
#define BEZ_PTS    3*BEZ_CURVES+1  //Number of Points is the #Curves * 3 + 1 


/*
** This structue defines a basic bezier curve.  It is intended to be used in
** an array of beziers.
*/
typedef struct _BEZBUFFER
{
	POINT pPts[BEZ_PTS];
} BEZBUFFER;
typedef BEZBUFFER      *LPBEZBUFFER;

typedef struct _POLYDATA
{
	int      nBezTotal;
	int      nBezCurr;
	int      nColor;
	POINT    pVel[BEZ_PTS];
	HANDLE   hBezBuffer;
} POLYDATA;
typedef POLYDATA   *LPPOLYDATA;

LPPOLYDATA ppd;
LPBEZBUFFER lpBez,lpCurr,lpPrev;
DWORD FAR lRandom(VOID);

int PolyNewVel(int i)
{
	int nRet;


	if ((i == 1) || (i == 2))
		nRet = (int)((lRandom() % VELMAX) / 3) + VELMIN;
	else
		nRet = (int)(lRandom() % VELMAX) + VELMIN;

	return((nRet < 0) ? -nRet : nRet);
}

DWORD FAR lRandom(VOID)
{
	static DWORD glSeed = (DWORD)-365387184;

	glSeed *= 69069;
	return(++glSeed);
}



VOID PolyDrawBez(HWND hWnd)
{
	//PPOLYDATA   ppd;
	//LPBEZBUFFER lpBez,lpCurr,lpPrev;
	int         idx,x,y;
	RECT        rect;
	HDC         hDC;
	HPEN        hPen;

	static COLORREF crColor[] = {0x000000FF,0x0000FF00,0x00FF0000,0x0000FFFF,
		0x00FF00FF,0x00FFFF00,0x00FFFFFF,0x00000080,
		0x00008000,0x00800000,0x00008080,0x00800080,
		0x00808000,0x00808080,0x000000FF,0x0000FF00,
		0x00FF0000,0x0000FFFF,0x00FF00FF,0x00FFFF00};


	//if(ppd = (PPOLYDATA)HeapAlloc(GetProcessHeap(),HEAP_NO_SERIALIZE,sizeof(POLYDATA)))
	//   {
	//       if(lpBez = (LPBEZBUFFER)GlobalLock(ppd->hBezBuffer))
	//       {
	if(hDC = GetDC(hWnd))
	{
		GetClientRect(hWnd,&rect);

		lpPrev = lpBez+ppd->nBezCurr;

		ppd->nBezCurr += 1;

		if(ppd->nBezCurr >= ppd->nBezTotal)
		{
			ppd->nBezCurr = 0;

			ppd->nColor  = (++ppd->nColor % 20);
		}
		lpCurr = lpBez+ppd->nBezCurr;


		if(lpCurr->pPts[0].x != -1)
		{
			hPen = SelectObject(hDC,GetStockObject(WHITE_PEN));
			//#if defined(_WIN32) && defined(WIN32)
			PolyBezier(hDC,lpCurr->pPts,BEZ_PTS);
			//#else
			//                    Polyline(hDC,lpCurr->pPts,BEZ_PTS);
			//#endif
			SelectObject(hDC,hPen);
		}

		for(idx=0; idx < BEZ_PTS; idx++)
		{
			x  = lpPrev->pPts[idx].x;
			y  = lpPrev->pPts[idx].y;
			x += ppd->pVel[idx].x;
			y += ppd->pVel[idx].y;

			if(x >= rect.right)
			{
				x = rect.right - ((x - rect.right)+1);

				ppd->pVel[idx].x = -PolyNewVel(idx);
			}

			if(x <= rect.left)
			{
				x = rect.left + ((rect.left - x)+1);
				ppd->pVel[idx].x = PolyNewVel(idx);
			}

			if(y >= rect.bottom)
			{
				y = rect.bottom - ((y - rect.bottom)+1);
				ppd->pVel[idx].y = -PolyNewVel(idx);
			}

			if(y <= rect.top)
			{
				y = rect.top + ((rect.top - y)+1);
				ppd->pVel[idx].y = PolyNewVel(idx);
			}

			lpCurr->pPts[idx].x = x;
			lpCurr->pPts[idx].y = y;

		}


		hPen = SelectObject(hDC,CreatePen(PS_SOLID,1,crColor[ppd->nColor]));
		//#if defined(_WIN32) && defined(WIN32)
		PolyBezier(hDC,lpCurr->pPts,BEZ_PTS);
		//#else
		//                Polyline(hDC,lpCurr->pPts,BEZ_PTS);
		//#endif
		DeleteObject(SelectObject(hDC,hPen));


		//#if defined(_WIN32) && defined(WIN32)
		SetROP2(hDC,R2_COPYPEN);
		//#endif
		ReleaseDC(hWnd,hDC);
		//    }
		//    GlobalUnlock(ppd->hBezBuffer);
		//}
		//UnlockWindowInfo(hWnd);
	}
}


VOID PolyRedraw(HWND hWnd)
{
	//PPOLYDATA   ppd;
	//LPBEZBUFFER lpBez,lpCurr;
	HDC         hDC;
	int         i,j;
	RECT        rect;


	//if(ppd = (PPOLYDATA)LockWindowInfo(hWnd))
	//{
	//    if(lpBez = (LPBEZBUFFER)GlobalLock(ppd->hBezBuffer))
	//    {
	if(hDC = GetDC(hWnd))
	{
		/*
		** Save the current bezier.  Set the first bezier in the
		** array to that curve, and use it as a basis for the next
		** series.
		*/
		lpCurr        = lpBez+ppd->nBezCurr;
		*lpBez        = *lpCurr;
		ppd->nBezCurr = 0;


		/*
		** Clean the curves (all but the first curve).
		*/
		for(j=1; j < ppd->nBezTotal; j++)
		{
			for(i=0; i < BEZ_PTS; i++)
			{
				(lpBez+j)->pPts[i].x = -1;
				(lpBez+j)->pPts[i].y = 0;
			}
		}



		/*
		** Clear the display.
		*/
		GetClientRect(hWnd,&rect);
		BitBlt(hDC,0,0,rect.right, rect.bottom,(HDC)0,0,0,WHITENESS);


		/*
		** Draw the first curve in the bezier array.
		*/
		//#if defined(_WIN32) && defined(WIN32)
		PolyBezier(hDC,lpBez->pPts,BEZ_PTS);
		//#else
		//                Polyline(hDC,lpBez->pPts,BEZ_PTS);
		//#endif
		ReleaseDC(hWnd,hDC);
		//    }
		//    GlobalUnlock(ppd->hBezBuffer);
		//}
		//UnlockWindowInfo(hWnd);
	}
	return;
}

VOID PolyPaintProc(HWND hWnd)
{
	HDC         hDC;
	PAINTSTRUCT ps;


	if(hDC = BeginPaint(hWnd,&ps))
		EndPaint(hWnd,&ps);

	PolyRedraw(hWnd);

	return;
}

VOID PolyInitPoints(HWND hWnd)
{
	//PPOLYDATA   ppd;
	//LPBEZBUFFER lpBez;
	int         idx;
	RECT        rect;


	//if(ppd = (PPOLYDATA)LockWindowInfo(hWnd))
	//{
	//    if(lpBez = (LPBEZBUFFER)GlobalLock(ppd->hBezBuffer))
	//    {
	GetClientRect(hWnd,&rect);

	for(idx=0; idx < BEZ_PTS-1; idx++)
	{
		lpBez->pPts[idx].x = lRandom() % rect.right;
		lpBez->pPts[idx].y = lRandom() % rect.bottom;

		ppd->pVel[idx].x = PolyNewVel(idx);
		ppd->pVel[idx].y = PolyNewVel(idx);
		//    }
		//    GlobalUnlock(ppd->hBezBuffer);
		//}
		//UnlockWindowInfo(hWnd);
	}
	return;
}


BOOL PolyCreateProc(HWND hWnd)
{
	ppd = HeapAlloc(GetProcessHeap(),HEAP_NO_SERIALIZE,sizeof(POLYDATA));
	lpBez = HeapAlloc(GetProcessHeap(),HEAP_NO_SERIALIZE,sizeof(BEZBUFFER));
	lpCurr = HeapAlloc(GetProcessHeap(),HEAP_NO_SERIALIZE,sizeof(BEZBUFFER));
	lpPrev = HeapAlloc(GetProcessHeap(),HEAP_NO_SERIALIZE,sizeof(BEZBUFFER));

	//if(AllocWindowInfo(hWnd,sizeof(POLYDATA)))
	//{
	//    if(ppd = (PPOLYDATA)LockWindowInfo(hWnd))
	//    {
	ppd->nBezTotal  = 20;
	ppd->nBezCurr   = 0;
	ppd->nColor     = 0;
	ppd->hBezBuffer = GlobalAlloc(GHND,(DWORD)(sizeof(BEZBUFFER) * MAX_BEZIER));

	//  UnlockWindowInfo(hWnd);
	if ( !ppd->hBezBuffer )
	{
		//FreeWindowInfo(hWnd);
		return(FALSE);
	}

	PolyInitPoints(hWnd);

	SetTimer(hWnd,1,50,NULL);
	return(TRUE);
	//}
	//    FreeWindowInfo(hWnd);
	//}
	return(FALSE);
}


LRESULT CALLBACK PolyProc(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
	switch(wMsg)
	{
	case WM_CREATE:
		PolyCreateProc(hWnd);
		break;


	case WM_COMMAND:
		//PolyCommandProc(hWnd,wParam,lParam);
		break;

	case WM_MOVE:
		PolyRedraw(hWnd);
		break;


	case WM_TIMER:
		PolyDrawBez(hWnd);
		break;


	case WM_PAINT:
		PolyPaintProc(hWnd);
		break;


	case WM_DESTROY:
		//PolyDestroyProc(hWnd);
		//FreeWindowInfo(hWnd);		
		HeapFree(GetProcessHeap(),HEAP_NO_SERIALIZE,lpBez);
		HeapFree(GetProcessHeap(),HEAP_NO_SERIALIZE,lpCurr);
		HeapFree(GetProcessHeap(),HEAP_NO_SERIALIZE,lpPrev);
		HeapFree(GetProcessHeap(),HEAP_NO_SERIALIZE,ppd);
		ExitProcess(0);
		break;


	default:
		return(DefWindowProc(hWnd,wMsg,wParam,lParam));
	}
	return(0l);
}


int WINAPI WinMain(HINSTANCE hinstance,
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

	// 注册窗口类，并创建窗口，用于显示截取的位图
	wcx.cbSize = sizeof(wcx);
	wcx.style = CS_HREDRAW |
		CS_VREDRAW;
	wcx.lpfnWndProc = PolyProc;
	wcx.cbClsExtra = 0;
	wcx.cbWndExtra = 0;
	wcx.hInstance = hinstance;
	wcx.hIcon = LoadIcon(NULL,	MAKEINTRESOURCE(IDI_APPLICATION));
	wcx.hCursor = LoadCursor(NULL,IDC_ARROW);
	wcx.hbrBackground = (HBRUSH)GetStockObject(COLOR_WINDOW+1);
	wcx.lpszMenuName = NULL;
	wcx.lpszClassName = "MainWClass";
	wcx.hIconSm = NULL;

	if( !RegisterClassEx(&wcx))
	{
		return 1;
	}
	// 创建窗口
	hwnd = CreateWindow(
		"MainWClass",
		"ploy line",
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS |
		WS_MAXIMIZE | WS_POPUPWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		800,
		600,
		(HWND) NULL,
		(HMENU) NULL,
		hinstance,
		(LPVOID) NULL);

	if (!hwnd)
	{
		return 1;
	}
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