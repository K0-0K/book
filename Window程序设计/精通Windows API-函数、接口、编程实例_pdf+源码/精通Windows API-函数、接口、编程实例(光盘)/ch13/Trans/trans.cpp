#include <windows.h>


enum
{
	A,
	B,
	SCALE,
	ROTATE,
	TRANSLATE,
	SHEAR,
	REFLECT,
	NORMAL,
	EXIT
};

int iStart = A;

void TransformAndDraw(int iTransform, HWND hWnd) 
{ 
    HDC hDC; 
    XFORM xForm; 
    RECT rect; 
 
    // Retrieve a DC handle for the application's window. 
 
    hDC = GetDC(hWnd); 
 
    // Set the mapping mode to LOENGLISH. This moves the 
    // client area origin from the upper left corner of the 
    // window to the lower left corner (this also reorients 
    // the y-axis so that drawing operations occur in a true 
    // Cartesian space). It guarantees portability so that 
    // the object drawn retains its dimensions on any display. 

    SetGraphicsMode(hDC, GM_ADVANCED);
    SetMapMode(hDC, MM_LOENGLISH); 
 
    // Set the appropriate world transformation (based on the 
    // user's menu selection). 
 
    switch (iTransform) 
    { 
	case A:
	case B:
            xForm.eM11 = (FLOAT) 1; 
            xForm.eM12 = (FLOAT) 0.0; 
            xForm.eM21 = (FLOAT) 0.0; 
            xForm.eM22 = (FLOAT) 1; 
            xForm.eDx  = (FLOAT) 0.0; 
            xForm.eDy  = (FLOAT) 0.0; 
            SetWorldTransform(hDC, &xForm); 
            break; 

        case SCALE:        // Scale to 1/2 of the original size. 
            xForm.eM11 = (FLOAT) 0.5; 
            xForm.eM12 = (FLOAT) 0.0; 
            xForm.eM21 = (FLOAT) 0.0; 
            xForm.eM22 = (FLOAT) 0.5; 
            xForm.eDx  = (FLOAT) 0.0; 
            xForm.eDy  = (FLOAT) 0.0; 
            SetWorldTransform(hDC, &xForm); 
            break; 
 
        case TRANSLATE:   // Translate right by 3/4 inch. 
            xForm.eM11 = (FLOAT) 1.0; 
            xForm.eM12 = (FLOAT) 0.0; 
            xForm.eM21 = (FLOAT) 0.0; 
            xForm.eM22 = (FLOAT) 1.0; 
            xForm.eDx  = (FLOAT) 75.0; 
            xForm.eDy  = (FLOAT) 0.0; 
            SetWorldTransform(hDC, &xForm); 
            break; 
 
        case ROTATE:      // Rotate 30 degrees counterclockwise. 
            xForm.eM11 = (FLOAT) 0.8660; 
            xForm.eM12 = (FLOAT) 0.5000; 
            xForm.eM21 = (FLOAT) -0.5000; 
            xForm.eM22 = (FLOAT) 0.8660; 
            xForm.eDx  = (FLOAT) 0.0; 
            xForm.eDy  = (FLOAT) 0.0; 
            SetWorldTransform(hDC, &xForm); 
            break; 
 
        case SHEAR:       // Shear along the x-axis with a 
                          // proportionality constant of 1.0. 
            xForm.eM11 = (FLOAT) 1.0; 
            xForm.eM12 = (FLOAT) 1.0; 
            xForm.eM21 = (FLOAT) 0.0; 
            xForm.eM22 = (FLOAT) 1.0; 
            xForm.eDx  = (FLOAT) 0.0; 
            xForm.eDy  = (FLOAT) 0.0; 
            SetWorldTransform(hDC, &xForm); 
            break; 
 
        case REFLECT:     // Reflect about a horizontal axis. 
            xForm.eM11 = (FLOAT) 1.0; 
            xForm.eM12 = (FLOAT) 0.0; 
            xForm.eM21 = (FLOAT) 0.0; 
            xForm.eM22 = (FLOAT) -1.0; 
            xForm.eDx  = (FLOAT) 0.0; 
            xForm.eDy  = (FLOAT) 0.0; 
            SetWorldTransform(hDC, &xForm); 
            break; 
 
        case NORMAL:      // Set the unity transformation. 
            xForm.eM11 = (FLOAT) 1.0; 
            xForm.eM12 = (FLOAT) 0.0; 
            xForm.eM21 = (FLOAT) 0.0; 
            xForm.eM22 = (FLOAT) 1.0; 
            xForm.eDx  = (FLOAT) 0.0; 
            xForm.eDy  = (FLOAT) 0.0; 
            SetWorldTransform(hDC, &xForm); 
            break; 
		case EXIT:
			ExitProcess(0); 
    } 
 
    // Find the midpoint of the client area. 
 
    GetClientRect(hWnd, (LPRECT) &rect); 
    DPtoLP(hDC, (LPPOINT) &rect, 2); 
 
    //// Select a hollow brush. 
 
    SelectObject(hDC, GetStockObject(HOLLOW_BRUSH)); 
	//SelectObject(hDC, GetStockObject(BLACK_BRUSH)); 

 
    // Draw the exterior circle. 
 
    //Ellipse(hDC, (rect.right / 2 - 100), (rect.bottom / 2 + 100), 
    //    (rect.right / 2 + 100), (rect.bottom / 2 - 100)); 

	    //Ellipse(hDC, 100, 100, 500, 300); 

	Ellipse(hDC, (rect.right / 2 - 200), (rect.bottom / 2 + 100), 
        (rect.right / 2 + 200), (rect.bottom / 2 - 100)); 
 
    // Draw the interior circle. 
 
    //Ellipse(hDC, (rect.right / 2 -94), (rect.bottom / 2 + 94), 
    //    (rect.right / 2 + 94), (rect.bottom / 2 - 94)); 
 
    //// Draw the key. 
 
    //Rectangle(hDC, (rect.right / 2 - 13), (rect.bottom / 2 + 113), 
    //    (rect.right / 2 + 13), (rect.bottom / 2 + 50)); 
    //Rectangle(hDC, (rect.right / 2 - 13), (rect.bottom / 2 + 96), 
    //    (rect.right / 2 + 13), (rect.bottom / 2 + 50)); 
 
    //// Draw the horizontal lines. 
 
    //MoveToEx(hDC, (rect.right/2 - 150), (rect.bottom / 2 + 0), NULL); 
    //LineTo(hDC, (rect.right / 2 - 16), (rect.bottom / 2 + 0)); 
 
    //MoveToEx(hDC, (rect.right / 2 - 13), (rect.bottom / 2 + 0), NULL); 
    //LineTo(hDC, (rect.right / 2 + 13), (rect.bottom / 2 + 0)); 
 
    //MoveToEx(hDC, (rect.right / 2 + 16), (rect.bottom / 2 + 0), NULL); 
    //LineTo(hDC, (rect.right / 2 + 150), (rect.bottom / 2 + 0)); 
 
    //// Draw the vertical lines. 
 
    //MoveToEx(hDC, (rect.right/2 + 0), (rect.bottom / 2 - 150), NULL); 
    //LineTo(hDC, (rect.right / 2 + 0), (rect.bottom / 2 - 16)); 
 
    //MoveToEx(hDC, (rect.right / 2 + 0), (rect.bottom / 2 - 13), NULL); 
    //LineTo(hDC, (rect.right / 2 + 0), (rect.bottom / 2 + 13)); 
 
    //MoveToEx(hDC, (rect.right / 2 + 0), (rect.bottom / 2 + 16), NULL); 
    //LineTo(hDC, (rect.right / 2 + 0), (rect.bottom / 2 + 150)); 
 
    ReleaseDC(hWnd, hDC); 
} 


BOOL CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, 
                          LPARAM lParam) 
{ 
 
    switch (uMsg) 
    { 

	case WM_LBUTTONDOWN: 			
			TransformAndDraw(iStart++, hwnd);
			 break;

        case WM_DESTROY: 
            PostQuitMessage(0); 
            break; 
 
        default: 
            return DefWindowProc(hwnd, uMsg, wParam, lParam); 
    } 
    return (LRESULT) NULL; 
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
	wc.lpfnWndProc = (WNDPROC)WndProc; 
	wc.cbClsExtra = 0; 
	wc.cbWndExtra = 0; 
	wc.hInstance = hInstance;
	wc.hIcon = NULL; 
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "trans"; 

	if (!RegisterClass(&wc))
	{
		return (FALSE);
	}

	hWnd = CreateWindow(
		"trans", 
		"trans", 
		WS_OVERLAPPEDWINDOW,
		0, 0, 500, 500, 
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